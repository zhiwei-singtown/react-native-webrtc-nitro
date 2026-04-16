#pragma once
#include "FecEncoder.hpp" // for FEC_HEADER_SIZE, RTP_FIXED_HEADER_SIZE
#include <map>

namespace rtc
{

    class RTC_CPP_EXPORT FecDecoder final : public MediaHandler
    {
      public:
        FecDecoder (uint8_t fecPayloadType = 127);
        void incoming (message_vector &messages,
                       const message_callback &send) override;

      private:
        uint8_t mFecPayloadType;
        // Recent data packets indexed by seq number for FEC recovery
        std::map<uint16_t, message_ptr> mRecentPackets;

        void pruneOldPackets (uint16_t currentSeq);
        auto recoverPacket (const uint8_t *fecData, size_t fecSize,
                            uint16_t baseSeq, uint8_t groupSize)
            -> message_ptr;
    };

} // namespace rtc
