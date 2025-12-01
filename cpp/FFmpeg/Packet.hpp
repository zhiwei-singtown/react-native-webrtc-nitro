#pragma once
#include "Common.hpp"

namespace FFmpeg
{
    class Packet
    {
      private:
        std::shared_ptr<AVPacket> packet;

      public:
        Packet ();
        Packet (size_t size);

        auto operator->() -> AVPacket * { return packet.get (); }

        auto operator->() const -> const AVPacket * { return packet.get (); }

        auto get () -> AVPacket * { return packet.get (); }

        [[nodiscard]] auto get () const -> const AVPacket *
        {
            return packet.get ();
        }
    };
} // namespace FFmpeg
