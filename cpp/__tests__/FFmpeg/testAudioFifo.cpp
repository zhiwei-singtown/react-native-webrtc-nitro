#include "FFmpeg.hpp"
#include <gtest/gtest.h>

using namespace FFmpeg;

constexpr int SAMPLE_RATE = 48000;
constexpr int NB_SAMPLES = 960;

TEST (AudioFifoTest, testNormal)
{
    AudioFifo fifo;
    Frame inputFrame (AV_SAMPLE_FMT_FLT, SAMPLE_RATE, 1, NB_SAMPLES);
    inputFrame.fillNoise ();

    fifo.write (inputFrame);
    std::optional<Frame> outFrame = fifo.read (NB_SAMPLES);

    ASSERT_TRUE (outFrame.has_value ()) << "Expected to read frame from FIFO";
    ASSERT_EQ (outFrame.value ()->nb_samples, NB_SAMPLES);
    ASSERT_EQ (outFrame.value ()->format, AV_SAMPLE_FMT_FLT);
    ASSERT_EQ (outFrame.value ()->sample_rate, SAMPLE_RATE);
    ASSERT_EQ (outFrame.value ()->ch_layout.nb_channels, 1);
}

TEST (AudioFifoTest, testPts)
{
    AudioFifo fifo;
    Frame in1 (AV_SAMPLE_FMT_FLT, SAMPLE_RATE, 1, NB_SAMPLES, 1);
    Frame in2 (AV_SAMPLE_FMT_FLT, SAMPLE_RATE, 1, NB_SAMPLES, NB_SAMPLES + 1);

    fifo.write (in1);
    fifo.write (in2);

    std::optional<Frame> out1 = fifo.read (NB_SAMPLES);
    ASSERT_TRUE (out1.has_value ()) << "Expected first frame";
    ASSERT_EQ (out1.value ()->pts, 1);

    auto out2 = fifo.read (NB_SAMPLES);
    ASSERT_TRUE (out2.has_value ()) << "Expected second frame";
    ASSERT_EQ (out2.value ()->pts, 961);
}

TEST (AudioFifoTest, testEmpty)
{
    AudioFifo fifo;
    std::optional<Frame> outFrame = fifo.read (NB_SAMPLES);
    ASSERT_FALSE (outFrame.has_value ()) << "Should not read from empty FIFO";
}

TEST (AudioFifoTest, testPartialRead)
{
    AudioFifo fifo;
    Frame inputFrame (AV_SAMPLE_FMT_FLT, SAMPLE_RATE, 1, NB_SAMPLES);
    inputFrame.fillNoise ();

    fifo.write (inputFrame);
    std::optional<Frame> outFrame = fifo.read (NB_SAMPLES / 2); // read half
    ASSERT_TRUE (outFrame.has_value ());
    ASSERT_EQ (outFrame.value ()->nb_samples, NB_SAMPLES / 2);
}

TEST (AudioFifoTest, testMultipleWrites)
{
    AudioFifo fifo;
    constexpr int FRAME_NUMBERS = 5;

    for (int i = 0; i < FRAME_NUMBERS; i++)
    {
        Frame frame (AV_SAMPLE_FMT_FLT, SAMPLE_RATE, 1, NB_SAMPLES);
        frame.fillNoise ();
        fifo.write (frame);
    }

    for (int i = 0; i < FRAME_NUMBERS; i++)
    {
        std::optional<Frame> outFrame = fifo.read (NB_SAMPLES);
        ASSERT_TRUE (outFrame.has_value ()) << "Failed at iteration " << i;
    }
}
