#pragma once
#include "Frame.hpp"

namespace FFmpeg
{
    class Scaler
    {
      private:
        std::recursive_mutex mutex;
        SwsContext *sws_ctx = nullptr;

      public:
        ~Scaler ();

        auto scale (const Frame &frame, AVPixelFormat format, int width,
                    int height) -> Frame;
    };
} // namespace FFmpeg
