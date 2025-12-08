#pragma once
#include "HybridRTCRtpReceiver.hpp"
#include "HybridRTCRtpSender.hpp"
#include "HybridRTCRtpTransceiverSpec.hpp"
#include <rtc/rtc.hpp>

namespace margelo::nitro::webrtc
{
    class HybridRTCRtpTransceiver : public HybridRTCRtpTransceiverSpec
    {
      private:
        std::optional<rtc::Description::Media::RtpMap> rtpMap;
        const std::vector<std::string> sendMsids;
        std::shared_ptr<rtc::Track> track;
        const std::shared_ptr<rtc::PeerConnection> peerConnection;

      public:
        std::optional<std::string> mid = std::nullopt;
        const std::string kind;
        const rtc::Description::Direction direction;

        const std::shared_ptr<HybridRTCRtpSender> hybridRtcRtpSender;
        const std::shared_ptr<HybridRTCRtpReceiver> hybridRtcRtpReceiver;

        HybridRTCRtpTransceiver ()
            : HybridObject (TAG), HybridRTCRtpTransceiverSpec (),
              peerConnection (nullptr), kind (""),
              direction (rtc::Description::Direction::Inactive)

        {
            throw std::runtime_error (
                "Uncaught TypeError: Illegal constructor");
        }

        HybridRTCRtpTransceiver (
            const std::shared_ptr<rtc::PeerConnection> &peerConnection,
            const std::shared_ptr<HybridMediaStreamTrack> &mediaStreamTrack,
            rtc::Description::Direction direction,
            const std::vector<std::string> &msids)
            : HybridObject (TAG), HybridRTCRtpTransceiverSpec (),
              sendMsids (msids), peerConnection (peerConnection),
              kind (mediaStreamTrack->getKind ()), direction (direction),
              hybridRtcRtpSender (std::make_shared<HybridRTCRtpSender> ()),
              hybridRtcRtpReceiver (std::make_shared<HybridRTCRtpReceiver> ())
        {
            if (direction == rtc::Description::Direction::SendOnly
                || direction == rtc::Description::Direction::SendRecv)
            {
                hybridRtcRtpSender->mediaStreamTrack = mediaStreamTrack;
            }
        }

        auto offerMedia (const std::string &mid) -> rtc::Description::Media;
        auto answerMedia (const rtc::Description::Media &remoteMedia)
            -> rtc::Description::Media;

        void senderOnOpen ();
        void receiverOnOpen ();
        void start (const rtc::Description::Media &remoteMedia);

        auto getMid () -> std::variant<nitro::NullType, std::string> override
        {
            if (mid.has_value ())
            {
                return mid.value ();
            }
            else
            {
                return nitro::null;
            }
        };

        auto getSender () -> std::shared_ptr<HybridRTCRtpSenderSpec> override
        {
            return hybridRtcRtpSender;
        };

        auto getReceiver ()
            -> std::shared_ptr<HybridRTCRtpReceiverSpec> override
        {
            return hybridRtcRtpReceiver;
        };

        auto getDirection () -> RTCRtpTransceiverDirection override
        {
            switch (direction)
            {
            case rtc::Description::Direction::SendOnly:
                return RTCRtpTransceiverDirection::SENDONLY;
            case rtc::Description::Direction::RecvOnly:
                return RTCRtpTransceiverDirection::RECVONLY;
            case rtc::Description::Direction::SendRecv:
                return RTCRtpTransceiverDirection::SENDRECV;
            case rtc::Description::Direction::Inactive:
                return RTCRtpTransceiverDirection::INACTIVE;
            default:
                throw std::runtime_error ("Unknown direction");
            }
        };
    };
} // namespace margelo::nitro::webrtc
