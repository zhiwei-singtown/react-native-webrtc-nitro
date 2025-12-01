#include "FFmpeg.hpp"
#include <filesystem>
#include <gtest/gtest.h>

using namespace FFmpeg;

constexpr int AUDIO_SAMPLE_RATE = 48000;
constexpr int AUDIO_NB_SAMPLES = 2048;
constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

TEST (MuxerTest, testMuxerAAC)
{
    std::string file = testing::TempDir () + "/test_file.mp4";
    Muxer muxer (file, AV_CODEC_ID_AAC, AV_CODEC_ID_NONE);

    Frame inputFrame (AV_SAMPLE_FMT_FLT, AUDIO_SAMPLE_RATE, 1,
                      AUDIO_NB_SAMPLES);
    inputFrame.fillNoise ();
    muxer.writeAudio (inputFrame);
    muxer.stop ();

    EXPECT_TRUE (std::filesystem::exists (file));
}

TEST (MuxerTest, testMuxerH264)
{
    std::string file = testing::TempDir () + "/test_file.mp4";

    Muxer muxer (file, AV_CODEC_ID_NONE, AV_CODEC_ID_H264);
    Frame inputFrame (AV_PIX_FMT_NV12, WIDTH, HEIGHT);
    inputFrame.fillNoise ();
    muxer.writeVideo (inputFrame);
    muxer.stop ();

    EXPECT_TRUE (std::filesystem::exists (file));
}

TEST (MuxerTest, testMuxerVideo)
{
    std::string file = testing::TempDir () + "/test_file.mp4";
    Muxer muxer (file, AV_CODEC_ID_AAC, AV_CODEC_ID_H264);

    Frame audioFrame (AV_SAMPLE_FMT_FLT, AUDIO_SAMPLE_RATE, 1,
                      AUDIO_NB_SAMPLES);
    Frame videoFrame (AV_PIX_FMT_NV12, WIDTH, HEIGHT);
    audioFrame.fillNoise ();
    videoFrame.fillNoise ();
    muxer.writeAudio (audioFrame);
    muxer.writeVideo (videoFrame);

    muxer.stop ();
    EXPECT_TRUE (std::filesystem::exists (file));
}

TEST (MuxerTest, testSaveMp4)
{
    std::string file = "output.mp4";
    Muxer muxer (file, AV_CODEC_ID_AAC, AV_CODEC_ID_H264);
    constexpr int LOOPS = 1000;
    constexpr int VIDEO_SAMPLES = 3000;

    for (int i = 0; i < LOOPS; ++i)
    {
        Frame audioFrame (AV_SAMPLE_FMT_FLT, AUDIO_SAMPLE_RATE, 1,
                          AUDIO_NB_SAMPLES, i * AUDIO_NB_SAMPLES);
        Frame videoFrame (AV_PIX_FMT_NV12, WIDTH, HEIGHT, i * VIDEO_SAMPLES);
        audioFrame.fillNoise ();
        videoFrame.fillNoise ();
        muxer.writeAudio (audioFrame);
        muxer.writeVideo (videoFrame);
    }
    muxer.stop ();
    EXPECT_TRUE (std::filesystem::exists (file));
}
