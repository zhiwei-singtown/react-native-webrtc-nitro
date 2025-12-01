#include "FFmpeg.hpp"
#include <gtest/gtest.h>

using namespace FFmpeg;

constexpr int AUDIO_SAMPLE_RATE = 48000;
constexpr int VIDEO_SAMPLE_RATE = 90000;
constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;
constexpr int TIME_MS = 1000;

void sleep_ms (int time_ms)
{
    using namespace std::chrono;

    auto start = high_resolution_clock::now ();
    auto end = start + milliseconds (time_ms);

    while (high_resolution_clock::now () < end)
    {
    }
}

TEST (currentPtsTest, test90k)
{
    const AVRational timebase = { 1, VIDEO_SAMPLE_RATE };

    int64_t pts1 = currentPts (timebase);
    sleep_ms (TIME_MS);
    int64_t pts2 = currentPts (timebase);

    ASSERT_NEAR (pts2 - pts1, VIDEO_SAMPLE_RATE, 90);
}

TEST (currentPtsTest, test48K)
{
    const AVRational timebase = { 1, AUDIO_SAMPLE_RATE };

    auto pts1 = currentPts (timebase);
    sleep_ms (TIME_MS);
    auto pts2 = currentPts (timebase);

    ASSERT_NEAR (pts2 - pts1, AUDIO_SAMPLE_RATE, 48);
}
