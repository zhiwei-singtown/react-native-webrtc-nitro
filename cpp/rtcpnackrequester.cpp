#include "rtcpnackrequester.hpp"

namespace rtc
{
    RtcpNackRequester::RtcpNackRequester (SSRC ssrc, size_t jitterSize,
                                          size_t nackWaitMs)
        : ssrc (ssrc), jitterSize (jitterSize), nackWaitMs (nackWaitMs)
    {
    }

    void RtcpNackRequester::incoming (message_vector &messages,
                                      const message_callback &send)
    {

        message_vector result;
        for (const auto &message : messages)
        {
            if (message->type != Message::Binary)
            {
                result.push_back (message);
                continue;
            }

            if (message->size () < sizeof (RtpHeader))
            {
                result.push_back (message);
                continue;
            }

            auto rtp = reinterpret_cast<RtpHeader *> (message->data ());
            uint16_t seqNo = rtp->seqNumber ();

            lostSequenceNumbers.erase (seqNo);
            if (expectSequence == 0)
            {
                expectSequence = seqNo;
            }
            if ((int16_t)(seqNo - expectSequence) >= 0)
            {
                receivePackets[seqNo] = message;
            }
        }

        while (receivePackets.size () > jitterSize)
        {
            bool alreadyReceived = receivePackets.count (expectSequence) > 0;
            if (alreadyReceived)
            {
                auto packet = receivePackets[expectSequence];
                result.push_back (packet);
                receivePackets.erase (expectSequence);
                expectSequence++;
                continue;
            }
            else
            {
                bool alreadySentNack
                    = lostSequenceNumbers.count (expectSequence) > 0;
                auto now = std::chrono::steady_clock::now ();
                if (alreadySentNack)
                {
                    if (now >= nackWaitUntil)
                    {
                        expectSequence++;
                    }
                }
                else
                {
                    lostSequenceNumbers.insert (expectSequence);
                    nackWaitUntil
                        = now + std::chrono::milliseconds (nackWaitMs);
                    send (nackMesssage (expectSequence));
                }
                break;
            }
        }
        messages.swap (result);
    }

    auto RtcpNackRequester::nackMesssage (uint16_t sequence) -> message_ptr
    {
        unsigned int fciCount = 0;
        uint16_t fciPID = 0;

        message_ptr message
            = make_message (RtcpNack::Size (1), Message::Control);
        auto *nack = reinterpret_cast<RtcpNack *> (message->data ());
        nack->preparePacket (ssrc, 1);
        nack->addMissingPacket (&fciCount, &fciPID, sequence);

        return message;
    }

} // namespace rtc
