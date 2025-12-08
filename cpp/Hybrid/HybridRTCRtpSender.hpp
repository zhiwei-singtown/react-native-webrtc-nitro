#pragma once
#include "HybridMediaStream.hpp"
#include "HybridMediaStreamTrack.hpp"
#include "HybridRTCRtpSenderSpec.hpp"

namespace margelo::nitro::webrtc
{
    class HybridRTCRtpSender : public HybridRTCRtpSenderSpec
    {
      public:
        std::shared_ptr<HybridMediaStreamTrack> mediaStreamTrack = nullptr;

        HybridRTCRtpSender () : HybridObject (TAG), HybridRTCRtpSenderSpec ()
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
