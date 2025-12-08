#pragma once
#include "HybridMediaStream.hpp"
#include "HybridMediaStreamTrack.hpp"
#include "HybridRTCRtpReceiverSpec.hpp"
#include <rtc/rtc.hpp>

namespace margelo::nitro::webrtc
{
    class HybridRTCRtpReceiver : public HybridRTCRtpReceiverSpec
    {
      public:
        std::shared_ptr<HybridMediaStreamTrack> mediaStreamTrack = nullptr;

        HybridRTCRtpReceiver ()
            : HybridObject (TAG), HybridRTCRtpReceiverSpec ()
        {
        }

        auto getTrack () -> std::variant<
            nitro::NullType,
            std::shared_ptr<HybridMediaStreamTrackSpec>> override
        {
            if (mediaStreamTrack == nullptr)
            {
                return nitro::null;
            }
            else
            {
                return mediaStreamTrack;
            }
        };
    };
} // namespace margelo::nitro::webrtc
