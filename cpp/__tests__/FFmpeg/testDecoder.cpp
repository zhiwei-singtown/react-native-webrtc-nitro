#include "FFmpeg.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <thread>

using namespace FFmpeg;

constexpr int SAMPLE_RATE = 48000;
constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

TEST (DecoderTest, testOpus)
{
    constexpr int NB_SAMPLES = 960;
    constexpr int NB_FRAMES = 10;
    constexpr int TIME_DELAY_MS = 20;

    Encoder encoder (AV_CODEC_ID_OPUS);
    for (int i = 0; i < NB_FRAMES; ++i)
    {
        Frame inputFrame (AV_SAMPLE_FMT_FLTP, SAMPLE_RATE, 2, NB_SAMPLES);
        inputFrame.fillNoise ();
        encoder.send (inputFrame);
        std::this_thread::sleep_for (
            std::chrono::milliseconds (TIME_DELAY_MS));
    }
    encoder.flush ();
    std::vector<Packet> packets = encoder.receive ();

    Decoder decoder (AV_CODEC_ID_OPUS);
    for (const auto &packet : packets)
    {
        decoder.send (packet);
    }
    decoder.flush ();
    std::vector<Frame> frames = decoder.receive ();
    ASSERT_GT (frames.size (), 0);
}

TEST (EncoderTest, testH264)
{
    constexpr int NB_FRAMES = 10;
    Encoder encoder (AV_CODEC_ID_H264);
    for (int i = 0; i < NB_FRAMES; ++i)
    {
        Frame inputFrame (AV_PIX_FMT_NV12, WIDTH, HEIGHT);
        inputFrame.fillNoise ();
        encoder.send (inputFrame);
    }
    encoder.flush ();
    std::vector<Packet> packets = encoder.receive ();

    Decoder decoder (AV_CODEC_ID_H264);
    for (const auto &packet : packets)
    {
        decoder.send (packet);
    }
    decoder.flush ();
    std::vector<Frame> frames = decoder.receive ();
    ASSERT_GT (frames.size (), 0);
}

TEST (EncoderTest, testH265)
{
    constexpr int NB_FRAMES = 10;
    Encoder encoder (AV_CODEC_ID_H265);
    for (int i = 0; i < NB_FRAMES; ++i)
    {
        Frame inputFrame (AV_PIX_FMT_NV12, WIDTH, HEIGHT);
        inputFrame.fillNoise ();
        encoder.send (inputFrame);
    }
    encoder.flush ();
    std::vector<Packet> packets = encoder.receive ();

    Decoder decoder (AV_CODEC_ID_H265);
    for (const auto &packet : packets)
    {
        decoder.send (packet);
    }
    decoder.flush ();
    std::vector<Frame> frames = decoder.receive ();
    ASSERT_GT (frames.size (), 0);
}
