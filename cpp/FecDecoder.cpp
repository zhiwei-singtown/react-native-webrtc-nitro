#include "FecDecoder.hpp"
#include <algorithm>
#include <cstring>

#ifdef __ANDROID__
#include <android/log.h>
#define FEC_LOG(...)                                                           \
    __android_log_print (ANDROID_LOG_DEBUG, "WebRTC FecDecoder", __VA_ARGS__)
#else
#define FEC_LOG(...)
#endif

namespace rtc
{

    FecDecoder::FecDecoder (uint8_t fecPayloadType)
        : mFecPayloadType (fecPayloadType)
    {
    }

    void FecDecoder::incoming (message_vector &messages,
                               const message_callback &send)
    {
        message_vector result;

        for (const auto &message : messages)
        {
            if (message->type != Message::Binary
                || message->size () < RTP_FIXED_HEADER_SIZE)
            {
                result.push_back (message);
                continue;
            }

            auto *rtp
                = reinterpret_cast<const RtpHeader *> (message->data ());
            uint8_t pt = rtp->payloadType ();

            if (pt == mFecPayloadType)
            {
                // FEC packet — try to recover missing data packet
                size_t minFecSize
                    = RTP_FIXED_HEADER_SIZE + FEC_HEADER_SIZE;
                if (message->size () < minFecSize)
                {
                    continue; // malformed FEC, skip
                }

                auto *fecData = reinterpret_cast<const uint8_t *> (
                    message->data ());
                const uint8_t *fecHeader
                    = fecData + RTP_FIXED_HEADER_SIZE;

                // Parse FEC header
                uint16_t baseSeq = (static_cast<uint16_t> (fecHeader[0])
                                    << 8)
                                   | fecHeader[1];
                uint8_t groupSize = fecHeader[2];

                if (groupSize == 0 || groupSize > 20)
                {
                    continue; // invalid
                }

                // Count how many data packets we have from this group
                size_t received = 0;
                uint16_t missingSeq = 0;
                for (uint8_t i = 0; i < groupSize; i++)
                {
                    uint16_t seq = baseSeq + i;
                    if (mRecentPackets.count (seq) > 0)
                    {
                        received++;
                    }
                    else
                    {
                        missingSeq = seq;
                    }
                }

                if (received == groupSize)
                {
                    // All packets present, no recovery needed
                    FEC_LOG ("group base=%u all %u present, skip",
                             (unsigned)baseSeq, (unsigned)groupSize);
                }
                else if (received == groupSize - 1)
                {
                    // Exactly one missing — recover it
                    auto recovered = recoverPacket (
                        fecData, message->size (), baseSeq, groupSize);
                    if (recovered)
                    {
                        FEC_LOG ("RECOVERED seq=%u (group base=%u)",
                                 (unsigned)missingSeq,
                                 (unsigned)baseSeq);
                        // Store recovered packet for future groups
                        mRecentPackets[missingSeq] = recovered;
                        result.push_back (recovered);
                    }
                }
                else
                {
                    FEC_LOG ("group base=%u missing %zu packets, "
                             "cannot recover",
                             (unsigned)baseSeq,
                             (size_t)(groupSize - received));
                }

                // FEC packets are NOT passed downstream
                continue;
            }

            // Data packet — pass through and store
            uint16_t seqNo = rtp->seqNumber ();
            mRecentPackets[seqNo] = message;
            pruneOldPackets (seqNo);
            result.push_back (message);
        }

        messages.swap (result);
    }

    auto FecDecoder::recoverPacket (const uint8_t *fecData,
                                    size_t fecSize, uint16_t baseSeq,
                                    uint8_t groupSize) -> message_ptr
    {
        const uint8_t *fecHeader = fecData + RTP_FIXED_HEADER_SIZE;

        // [3..14] XOR of RTP headers
        const uint8_t *headerXor = fecHeader + 3;

        // [15..16] XOR of payload lengths
        uint16_t lengthXor
            = (static_cast<uint16_t> (fecHeader[15]) << 8)
              | fecHeader[16];

        // XOR payload starts at offset 17 in FEC header
        const uint8_t *xorPayload
            = fecData + RTP_FIXED_HEADER_SIZE + FEC_HEADER_SIZE;
        size_t xorPayloadSize
            = fecSize - RTP_FIXED_HEADER_SIZE - FEC_HEADER_SIZE;

        // Recover the missing packet's RTP header by XORing with
        // all received packets' headers
        uint8_t recoveredHeader[RTP_FIXED_HEADER_SIZE];
        std::memcpy (recoveredHeader, headerXor, RTP_FIXED_HEADER_SIZE);

        // Recover payload length
        uint16_t recoveredLength = lengthXor;

        // Recover payload
        std::vector<uint8_t> recoveredPayload (xorPayloadSize);
        std::memcpy (recoveredPayload.data (), xorPayload,
                     xorPayloadSize);

        // XOR with all received data packets in the group
        for (uint8_t i = 0; i < groupSize; i++)
        {
            uint16_t seq = baseSeq + i;
            auto it = mRecentPackets.find (seq);
            if (it == mRecentPackets.end ())
            {
                continue; // this is the missing packet
            }

            const auto &pkt = it->second;
            auto *pktData
                = reinterpret_cast<const uint8_t *> (pkt->data ());
            auto *pktRtp
                = reinterpret_cast<const RtpHeader *> (pkt->data ());

            // XOR RTP header
            for (size_t j = 0; j < RTP_FIXED_HEADER_SIZE; j++)
            {
                recoveredHeader[j] ^= pktData[j];
            }

            // XOR payload length
            uint16_t pktPayloadLen = static_cast<uint16_t> (
                pkt->size () - pktRtp->getSize ());
            recoveredLength ^= pktPayloadLen;

            // XOR payload
            size_t headerSize = pktRtp->getSize ();
            const uint8_t *payload = pktData + headerSize;
            size_t payloadSize = pkt->size () - headerSize;
            for (size_t j = 0; j < payloadSize && j < xorPayloadSize;
                 j++)
            {
                recoveredPayload[j] ^= payload[j];
            }
        }

        // Validate recovered length
        if (recoveredLength > xorPayloadSize)
        {
            FEC_LOG ("recovered length %u exceeds max %zu, abort",
                     (unsigned)recoveredLength, xorPayloadSize);
            return nullptr;
        }

        // Build recovered packet: RTP header + payload
        size_t totalSize = RTP_FIXED_HEADER_SIZE + recoveredLength;
        auto recovered = make_message (totalSize, Message::Binary);
        auto *out
            = reinterpret_cast<uint8_t *> (recovered->data ());
        std::memcpy (out, recoveredHeader, RTP_FIXED_HEADER_SIZE);
        std::memcpy (out + RTP_FIXED_HEADER_SIZE,
                     recoveredPayload.data (), recoveredLength);

        return recovered;
    }

    void FecDecoder::pruneOldPackets (uint16_t currentSeq)
    {
        // Keep only packets within 256 seq numbers of current
        auto it = mRecentPackets.begin ();
        while (it != mRecentPackets.end ())
        {
            int16_t diff
                = static_cast<int16_t> (currentSeq - it->first);
            if (diff > 256)
            {
                it = mRecentPackets.erase (it);
            }
            else
            {
                ++it;
            }
        }
    }

} // namespace rtc
