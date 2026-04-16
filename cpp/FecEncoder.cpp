#include "FecEncoder.hpp"
#include <algorithm>
#include <cstring>

#ifdef __ANDROID__
#include <android/log.h>
#define FEC_ENC_LOG(...)                                                       \
    __android_log_print (ANDROID_LOG_DEBUG, "WebRTC FecEncoder", __VA_ARGS__)
#else
#define FEC_ENC_LOG(...)
#endif

namespace rtc
{

    FecEncoder::FecEncoder (uint8_t fecPayloadType, size_t groupSize)
        : mFecPayloadType (fecPayloadType), mGroupSize (groupSize)
    {
    }

    void FecEncoder::outgoing (message_vector &messages,
                               const message_callback &send)
    {
        FEC_ENC_LOG ("outgoing called, %zu messages", messages.size ());
        message_vector result;

        for (const auto &message : messages)
        {
            if (message->type != Message::Binary
                || message->size () < RTP_FIXED_HEADER_SIZE)
            {
                result.push_back (message);
                continue;
            }

            // Pass data packet through
            result.push_back (message);

            // Buffer a copy for FEC generation
            mGroupBuffer.push_back (message);

            // When group is full, generate FEC packet
            if (mGroupBuffer.size () >= mGroupSize)
            {
                auto fecPacket = generateFecPacket ();
                if (fecPacket)
                {
                    FEC_ENC_LOG ("generated FEC packet, size=%zu",
                                 fecPacket->size ());
                    result.push_back (fecPacket);
                }
                mGroupBuffer.clear ();
            }
        }

        messages.swap (result);
    }

    auto FecEncoder::generateFecPacket () -> message_ptr
    {
        if (mGroupBuffer.empty ())
            return nullptr;

        // Get base seq and find max payload size
        auto *firstRtp = reinterpret_cast<const RtpHeader *> (
            mGroupBuffer.front ()->data ());
        auto *lastRtp = reinterpret_cast<const RtpHeader *> (
            mGroupBuffer.back ()->data ());
        uint16_t baseSeq = firstRtp->seqNumber ();

        size_t maxPayloadSize = 0;
        for (const auto &pkt : mGroupBuffer)
        {
            auto *rtp
                = reinterpret_cast<const RtpHeader *> (pkt->data ());
            size_t payloadSize = pkt->size () - rtp->getSize ();
            maxPayloadSize = std::max (maxPayloadSize, payloadSize);
        }

        // Allocate FEC packet: RTP header + FEC header + XOR payload
        size_t fecSize
            = RTP_FIXED_HEADER_SIZE + FEC_HEADER_SIZE + maxPayloadSize;
        auto fecMessage = make_message (fecSize, Message::Binary);
        auto *fecData
            = reinterpret_cast<uint8_t *> (fecMessage->data ());
        std::memset (fecData, 0, fecSize);

        // Build RTP header for FEC packet (copy from last data packet)
        std::memcpy (fecData, lastRtp, RTP_FIXED_HEADER_SIZE);
        auto *fecRtp = reinterpret_cast<RtpHeader *> (fecData);
        fecRtp->setPayloadType (mFecPayloadType);
        fecRtp->setMarker (false);

        // FEC header starts after RTP header
        uint8_t *fecHeader = fecData + RTP_FIXED_HEADER_SIZE;

        // [0..1] base_seq (big-endian)
        fecHeader[0] = static_cast<uint8_t> (baseSeq >> 8);
        fecHeader[1] = static_cast<uint8_t> (baseSeq & 0xFF);

        // [2] group_size
        fecHeader[2] = static_cast<uint8_t> (mGroupBuffer.size ());

        // [3..14] XOR of 12-byte RTP headers
        uint8_t headerXor[RTP_FIXED_HEADER_SIZE] = {};
        for (const auto &pkt : mGroupBuffer)
        {
            auto *pktData
                = reinterpret_cast<const uint8_t *> (pkt->data ());
            for (size_t i = 0; i < RTP_FIXED_HEADER_SIZE; i++)
            {
                headerXor[i] ^= pktData[i];
            }
        }
        std::memcpy (fecHeader + 3, headerXor, RTP_FIXED_HEADER_SIZE);

        // [15..16] XOR of payload lengths (big-endian)
        uint16_t lengthXor = 0;
        for (const auto &pkt : mGroupBuffer)
        {
            auto *rtp
                = reinterpret_cast<const RtpHeader *> (pkt->data ());
            uint16_t payloadLen
                = static_cast<uint16_t> (pkt->size () - rtp->getSize ());
            lengthXor ^= payloadLen;
        }
        fecHeader[15] = static_cast<uint8_t> (lengthXor >> 8);
        fecHeader[16] = static_cast<uint8_t> (lengthXor & 0xFF);

        // [17..] XOR of all payloads (zero-padded to maxPayloadSize)
        uint8_t *xorPayload
            = fecData + RTP_FIXED_HEADER_SIZE + FEC_HEADER_SIZE;
        for (const auto &pkt : mGroupBuffer)
        {
            auto *rtp
                = reinterpret_cast<const RtpHeader *> (pkt->data ());
            size_t headerSize = rtp->getSize ();
            auto *payload
                = reinterpret_cast<const uint8_t *> (pkt->data ())
                  + headerSize;
            size_t payloadSize = pkt->size () - headerSize;
            for (size_t i = 0; i < payloadSize; i++)
            {
                xorPayload[i] ^= payload[i];
            }
            // Remaining bytes (zero-padded) XOR with 0 = no change
        }

        return fecMessage;
    }

} // namespace rtc
