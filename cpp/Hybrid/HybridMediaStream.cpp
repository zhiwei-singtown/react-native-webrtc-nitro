#include "HybridMediaStream.hpp"

using namespace margelo::nitro::webrtc;

auto HybridMediaStream::getTracks ()
    -> std::vector<std::shared_ptr<HybridMediaStreamTrackSpec>>
{
    std::vector<std::shared_ptr<HybridMediaStreamTrackSpec>>
        hybridMediaStreamTrackSpecs;
    hybridMediaStreamTrackSpecs.reserve (mediaStreamTrackMap.size ());

    for (const auto &[_, track] : mediaStreamTrackMap)
    {
        hybridMediaStreamTrackSpecs.push_back (track);
    }
    return hybridMediaStreamTrackSpecs;
};

auto HybridMediaStream::getAudioTracks ()
    -> std::vector<std::shared_ptr<HybridMediaStreamTrackSpec>>
{
    std::vector<std::shared_ptr<HybridMediaStreamTrackSpec>>
        hybridMediaStreamTrackSpecs;
    hybridMediaStreamTrackSpecs.reserve (mediaStreamTrackMap.size ());

    for (const auto &[_, track] : mediaStreamTrackMap)
    {
        if (track->getKind () == "audio")
        {
            hybridMediaStreamTrackSpecs.push_back (track);
        }
    }
    return hybridMediaStreamTrackSpecs;
};

auto HybridMediaStream::getVideoTracks ()
    -> std::vector<std::shared_ptr<HybridMediaStreamTrackSpec>>
{
    std::vector<std::shared_ptr<HybridMediaStreamTrackSpec>>
        hybridMediaStreamTrackSpecs;
    hybridMediaStreamTrackSpecs.reserve (mediaStreamTrackMap.size ());

    for (const auto &[_, track] : mediaStreamTrackMap)
    {
        if (track->getKind () == "video")
        {
            hybridMediaStreamTrackSpecs.push_back (track);
        }
    }
    return hybridMediaStreamTrackSpecs;
};

void HybridMediaStream::addTrack (
    const std::shared_ptr<HybridMediaStreamTrackSpec> &track)
{
    mediaStreamTrackMap[track->getId ()] = track;
};

void HybridMediaStream::removeTrack (
    const std::shared_ptr<HybridMediaStreamTrackSpec> &track)
{
    mediaStreamTrackMap.erase (track->getId ());
};
