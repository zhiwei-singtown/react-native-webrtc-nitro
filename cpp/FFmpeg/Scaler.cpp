#include "Scaler.hpp"

using namespace FFmpeg;

Scaler::~Scaler ()
{
    if (sws_ctx != nullptr)
    {
        sws_freeContext (sws_ctx);
        sws_ctx = nullptr;
    }
}

auto Scaler::scale (const Frame &frame, AVPixelFormat format, int width,
                    int height) -> Frame
{
    std::lock_guard lock (mutex);
    if (frame->format == format && frame->width == width
        && frame->height == height)
    {
        return frame;
    }

    Frame dst (format, width, height, frame->pts);
    sws_ctx = sws_getCachedContext (sws_ctx, frame->width, frame->height,
                                    (AVPixelFormat)frame->format, dst->width,
                                    dst->height, (AVPixelFormat)dst->format,
                                    SWS_BILINEAR, nullptr, nullptr, nullptr);

    int ret = sws_scale (sws_ctx, frame->data, frame->linesize, 0,
                         frame->height, dst->data, dst->linesize);
    checkError (ret, "sws_scale");
    return dst;
}
