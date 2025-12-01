#pragma once
#include "Encoder.hpp"
#include "Frame.hpp"

namespace FFmpeg
{
    class Muxer
    {
      private:
        std::recursive_mutex mutex;
        AVFormatContext *fmt_ctx = nullptr;
        AVIOContext *avio_ctx = nullptr;
        Encoder audioEncoder;
        Encoder videoEncoder;

        AVStream *audioStream = nullptr;
        AVStream *videoStream = nullptr;
        bool hasWroteHeader = false;

        auto hasAudio () -> bool;
        auto hasVideo () -> bool;
        auto isAudioOpened () -> bool;
        auto isVideoOpened () -> bool;

        void tryWriteHeader ();
        void tryWriteAudio ();
        void tryWriteVideo ();

      public:
        Muxer (const std::string &path, AVCodecID audioCodecId,
               AVCodecID videoCodecId);
        ~Muxer ();

        void writeAudio (const Frame &frame);
        void writeVideo (const Frame &frame);
        void stop ();
    };
} // namespace FFmpeg
