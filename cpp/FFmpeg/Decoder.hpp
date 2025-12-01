#pragma once
#include "Frame.hpp"
#include "Packet.hpp"

namespace FFmpeg
{
    class Decoder
    {
      private:
        std::recursive_mutex mutex;
        AVCodecContext *ctx = nullptr;
        std::vector<Frame> buffer;

        static constexpr int OPUS_BIT_RATE = 64000;
        static constexpr int OPUS_SAMPLE_RATE = 48000;

      public:
        Decoder (AVCodecID codecId);
        ~Decoder ();

        void send (const Packet &packet);
        auto receive () -> std::vector<Frame>;
        void flush ();
    };
} // namespace FFmpeg
