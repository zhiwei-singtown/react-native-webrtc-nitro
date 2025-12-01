#pragma once
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

extern "C"
{
#define AVMediaType FFmpeg_AVMediaType
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/imgutils.h>
#include <libavutil/log.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#undef AVMediaType
}

namespace FFmpeg
{
    void checkError (int ret, const std::string &message);
    auto currentPts (const AVRational &time_base) -> int64_t;
} // namespace FFmpeg
