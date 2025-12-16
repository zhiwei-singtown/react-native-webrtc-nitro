#include "HybridMediaStream.hpp"

using namespace margelo::nitro::webrtc;

auto HybridMediaStream::getTracks ()
    -> std::vector<std::shared_ptr<HybridMediaStreamTrackSpec>>
{
    std::vector<std::shared_ptr<HybridMediaStreamTrackSpec>>
        hybridMediaStreamTrackSpecs;
    hybridMediaStreamTrackSpecs.reserve (mediaStreamTracks.size ());

    for (const auto &track : mediaStreamTracks)
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
    hybridMediaStreamTrackSpecs.reserve (mediaStreamTracks.size ());

    for (const auto &track : mediaStreamTracks)
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
    hybridMediaStreamTrackSpecs.reserve (mediaStreamTracks.size ());

    for (const auto &track : mediaStreamTracks)
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
    if (std::find (mediaStreamTracks.begin (), mediaStreamTracks.end (), track)
        != mediaStreamTracks.end ())
    {
        return;
    }

    mediaStreamTracks.push_back (track);
};

void HybridMediaStream::removeTrack (
    const std::shared_ptr<HybridMediaStreamTrackSpec> &track)
{
    mediaStreamTracks.erase (std::remove (mediaStreamTracks.begin (),
                                          mediaStreamTracks.end (), track),
                             mediaStreamTracks.end ());
};
