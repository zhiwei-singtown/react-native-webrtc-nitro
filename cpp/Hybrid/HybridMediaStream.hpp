#pragma once
#include "HybridMediaStreamSpec.hpp"
#include "HybridMediaStreamTrack.hpp"

namespace margelo::nitro::webrtc
{
    class HybridMediaStream : public HybridMediaStreamSpec
    {
      private:
        const std::string id;
        std::vector<std::shared_ptr<HybridMediaStreamTrackSpec>>
            mediaStreamTracks;

      public:
        HybridMediaStream ()
            : HybridObject (TAG), HybridMediaStreamSpec (), id (uuidv4 ())
        {
        }

        HybridMediaStream (std::string id)
            : HybridObject (TAG), HybridMediaStreamSpec (), id (std::move (id))
        {
        }

        auto getId () -> std::string override { return id; };

        auto getTracks () -> std::vector<
            std::shared_ptr<HybridMediaStreamTrackSpec>> override;

        void addTrack (
            const std::shared_ptr<HybridMediaStreamTrackSpec> &track) override;

        void removeTrack (
            const std::shared_ptr<HybridMediaStreamTrackSpec> &track) override;

        auto getAudioTracks () -> std::vector<
            std::shared_ptr<HybridMediaStreamTrackSpec>> override;

        auto getVideoTracks () -> std::vector<
            std::shared_ptr<HybridMediaStreamTrackSpec>> override;
    };
} // namespace margelo::nitro::webrtc
