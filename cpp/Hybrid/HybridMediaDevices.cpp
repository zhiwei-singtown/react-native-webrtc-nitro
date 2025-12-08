#include "HybridMediaDevices.hpp"
#include "MockCamera.hpp"
#include "MockMicrophone.hpp"

using namespace margelo::nitro::webrtc;

auto
HybridMediaDevices::getMockMedia (const MediaStreamConstraints &constraints)
    -> std::shared_ptr<Promise<std::shared_ptr<HybridMediaStreamSpec>>>
{
    auto hybridMediaStreams = std::make_shared<HybridMediaStream> ();
    if (constraints.audio.value_or (false))
    {
        auto hybridAudioTrack
            = std::make_shared<HybridMediaStreamTrack> ("audio");
        hybridAudioTrack->mockMicrophone = std::make_shared<MockMicrophone> (
            hybridAudioTrack->get_srcPipeId ());
        hybridMediaStreams->addTrack (hybridAudioTrack);
    }
    if (constraints.video.value_or (false))
    {
        auto hybridVideoTrack
            = std::make_shared<HybridMediaStreamTrack> ("video");
        hybridVideoTrack->mockCamera = std::make_shared<MockCamera> (
            hybridVideoTrack->get_srcPipeId ());
        hybridMediaStreams->addTrack (hybridVideoTrack);
    }
    return Promise<std::shared_ptr<HybridMediaStreamSpec>>::resolved (
        hybridMediaStreams);
};
