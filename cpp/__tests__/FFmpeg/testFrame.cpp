#include "FFmpeg.hpp"
#include <gtest/gtest.h>

using namespace FFmpeg;

TEST (FrameTest, testAudioFrame)
{
    constexpr int AUDIO_SAMPLE_RATE = 48000;
    constexpr int NB_SAMPLES = 960;
    const AVRational timebase = { 1, AUDIO_SAMPLE_RATE };

    int64_t ptsStart = currentPts (timebase);
    Frame frame (AV_SAMPLE_FMT_FLT, AUDIO_SAMPLE_RATE, 2, NB_SAMPLES);
    int64_t ptsEnd = currentPts (timebase);

    ASSERT_EQ (frame->format, AV_SAMPLE_FMT_FLT);
    ASSERT_EQ (frame->sample_rate, AUDIO_SAMPLE_RATE);
    ASSERT_EQ (frame->ch_layout.nb_channels, 2);
    ASSERT_EQ (frame->nb_samples, NB_SAMPLES);
    ASSERT_GE (frame->pts, ptsStart);
    ASSERT_LE (frame->pts, ptsEnd);
}

TEST (FrameTest, testVideoFrame)
{
    constexpr int WIDTH = 640;
    constexpr int HEIGHT = 480;
    constexpr int VIDEO_SAMPLE_RATE = 90000;
    const AVRational timebase = { 1, VIDEO_SAMPLE_RATE };

    int64_t ptsStart = currentPts (timebase);
    Frame frame (AV_PIX_FMT_YUV420P, WIDTH, HEIGHT);
    int64_t ptsEnd = currentPts (timebase);

    ASSERT_EQ (frame->format, AV_PIX_FMT_YUV420P);
    ASSERT_EQ (frame->width, WIDTH);
    ASSERT_EQ (frame->height, HEIGHT);
    ASSERT_GE (frame->pts, ptsStart);
    ASSERT_LE (frame->pts, ptsEnd);
}
