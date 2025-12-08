#pragma once
#include "HybridRTCPeerConnectionSpec.hpp"
#include "HybridRTCRtpTransceiver.hpp"

namespace margelo::nitro::webrtc
{
    using StateChangeHandler
        = std::optional<std::function<void (const Event &event)>>;
    using IceCandidateHandler = std::optional<
        std::function<void (const RTCPeerConnectionIceEvent &event)>>;
    using TrackHandler
        = std::optional<std::function<void (const RTCTrackEvent &event)>>;

    class HybridRTCPeerConnection : public HybridRTCPeerConnectionSpec
    {
      private:
        std::vector<std::shared_ptr<HybridRTCRtpTransceiver>>
            rtcRtpTransceivers;
        std::shared_ptr<rtc::PeerConnection> peerConnection;
        StateChangeHandler connectionStateChangeHandler;
        StateChangeHandler iceGatheringStateChangeHandler;
        IceCandidateHandler iceCandidateHandler;
        TrackHandler trackHandler;

        std::unordered_map<std::string, std::shared_ptr<HybridMediaStream>>
            receiveMediaStreamMap;
        auto getRemoteMediaFromMid (const std::string &mid)
            -> const rtc::Description::Media *;

        void offerTransceivers ();
        void answerTransceivers (const rtc::Description &remoteDescription);
        void startTransceivers ();

      public:
        HybridRTCPeerConnection ()
            : HybridObject (TAG), HybridRTCPeerConnectionSpec ()
        {
            peerConnection = std::make_shared<rtc::PeerConnection> ();
        }

        void setConfiguration (
            const std::optional<RTCConfiguration> &config) override;

        void close () override { peerConnection->close (); };

        auto getOnconnectionstatechange () -> StateChangeHandler override
        {
            return connectionStateChangeHandler;
        };

        void
        setOnconnectionstatechange (const StateChangeHandler &handler) override
        {
            connectionStateChangeHandler = handler;
        };

        auto getOnicegatheringstatechange () -> StateChangeHandler override
        {
            return iceGatheringStateChangeHandler;
        };

        void setOnicegatheringstatechange (
            const StateChangeHandler &handler) override
        {
            iceGatheringStateChangeHandler = handler;
        };

        auto getOnicecandidate () -> IceCandidateHandler override
        {
            return iceCandidateHandler;
        };

        void setOnicecandidate (const IceCandidateHandler &handler) override
        {
            iceCandidateHandler = handler;
        };

        auto getOntrack () -> TrackHandler override { return trackHandler; };

        void setOntrack (const TrackHandler &handler) override
        {
            trackHandler = handler;
        };

        auto getConnectionState () -> RTCPeerConnectionState override
        {
            switch (peerConnection->state ())
            {
            case rtc::PeerConnection::State::New:
                return RTCPeerConnectionState::NEW;
            case rtc::PeerConnection::State::Connecting:
                return RTCPeerConnectionState::CONNECTING;
            case rtc::PeerConnection::State::Connected:
                return RTCPeerConnectionState::CONNECTED;
            case rtc::PeerConnection::State::Disconnected:
                return RTCPeerConnectionState::DISCONNECTED;
            case rtc::PeerConnection::State::Failed:
                return RTCPeerConnectionState::FAILED;
            case rtc::PeerConnection::State::Closed:
                return RTCPeerConnectionState::CLOSED;
            }
        };

        auto getIceGatheringState () -> RTCIceGatheringState override
        {
            switch (peerConnection->gatheringState ())
            {
            case rtc::PeerConnection::GatheringState::New:
                return RTCIceGatheringState::NEW;
            case rtc::PeerConnection::GatheringState::InProgress:
                return RTCIceGatheringState::GATHERING;
            case rtc::PeerConnection::GatheringState::Complete:
                return RTCIceGatheringState::COMPLETE;
            }
        };

        auto getLocalDescription () -> std::string override
        {
            return peerConnection->localDescription ()->generateSdp ();
        };

        auto getRemoteDescription () -> std::string override
        {
            return peerConnection->remoteDescription ()->generateSdp ();
        };

        auto setLocalDescription (
            const std::optional<RTCSessionDescriptionInit> &description)
            -> std::shared_ptr<Promise<void>> override;
        auto
        setRemoteDescription (const RTCSessionDescriptionInit &description)
            -> std::shared_ptr<Promise<void>> override;
        auto addIceCandidate (
            const std::optional<
                std::variant<nitro::NullType, RTCIceCandidateInit>> &candidate)
            -> std::shared_ptr<Promise<void>> override;

        auto addTransceiver (
            const std::variant<std::shared_ptr<HybridMediaStreamTrackSpec>,
                               std::string> &trackOrKind,
            const std::optional<RTCRtpTransceiverInit> &init)
            -> std::shared_ptr<HybridRTCRtpTransceiverSpec> override;

        auto getTransceivers () -> std::vector<
            std::shared_ptr<HybridRTCRtpTransceiverSpec>> override;

        auto createOffer ()
            -> std::shared_ptr<Promise<RTCSessionDescriptionInit>> override;
        auto createAnswer ()
            -> std::shared_ptr<Promise<RTCSessionDescriptionInit>> override;
    };
} // namespace margelo::nitro::webrtc
