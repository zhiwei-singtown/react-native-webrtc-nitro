#pragma once
#include "Frame.hpp"

namespace FFmpeg
{
    class AudioFifo
    {
      private:
        std::recursive_mutex mutex;
        AVAudioFifo *fifo = nullptr;

        AVSampleFormat format = AV_SAMPLE_FMT_NONE;
        int channels = 0;
        int sample_rate = 0;
        int64_t pts = -1;

      public:
        ~AudioFifo ();

        void write (const Frame &frame);
        auto read (int nb_samples) -> std::optional<Frame>;
        void clear ();
    };
} // namespace FFmpeg
