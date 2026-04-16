#pragma once
#include <rtc/rtc.hpp>
#include <vector>

namespace rtc
{

    // FEC header layout (17 bytes):
    //   [0..1]   base_seq (big-endian)
    //   [2]      group_size
    //   [3..14]  XOR of 12-byte RTP headers
    //   [15..16] XOR of payload lengths (big-endian)
    //   [17..]   XOR of all payloads (zero-padded to max)
    static constexpr size_t FEC_HEADER_SIZE = 17;
    static constexpr size_t RTP_FIXED_HEADER_SIZE = 12;

    class RTC_CPP_EXPORT FecEncoder final : public MediaHandler
    {
      public:
        FecEncoder (uint8_t fecPayloadType = 127, size_t groupSize = 5);
        void outgoing (message_vector &messages,
                       const message_callback &send) override;

      private:
        uint8_t mFecPayloadType;
        size_t mGroupSize;
        std::vector<message_ptr> mGroupBuffer;

        auto generateFecPacket () -> message_ptr;
    };

} // namespace rtc
