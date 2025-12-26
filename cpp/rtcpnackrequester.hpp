#ifndef RTC_RTCP_JITTER_REQUESTER_H
#define RTC_RTCP_JITTER_REQUESTER_H

#include <queue>
#include <rtc/rtc.hpp>

namespace rtc
{

    class RTC_CPP_EXPORT RtcpNackRequester final : public MediaHandler
    {
      public:
        RtcpNackRequester (SSRC ssrc, size_t jitterSize = 5,
                           size_t nackWaitMs = 50);
        SSRC ssrc;
        void incoming (message_vector &messages,
                       const message_callback &send) override;

      private:
        size_t jitterSize;
        size_t nackWaitMs;

        uint16_t expectSequence = 0;
        std::chrono::steady_clock::time_point nackWaitUntil;

        std::map<uint16_t, message_ptr> receivePackets;
        std::set<uint16_t> lostSequenceNumbers;

        auto nackMesssage (uint16_t sequence) -> message_ptr;
    };

} // namespace rtc

#endif /* RTC_RTCP_JITTER_REQUESTER_H */
