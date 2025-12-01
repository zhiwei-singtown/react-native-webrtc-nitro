#include "FFmpeg.hpp"
#include <gtest/gtest.h>

using namespace FFmpeg;

constexpr int WIDTH_IN = 640;
constexpr int HEIGHT_IN = 480;
constexpr int WIDTH_OUT = 320;
constexpr int HEIGHT_OUT = 240;

TEST (ScalerTest, test_RGB24_to_NV12)
{
    Scaler scaler;
    Frame inputFrame (AV_PIX_FMT_RGB24, WIDTH_IN, HEIGHT_IN);
    Frame outFrame
        = scaler.scale (inputFrame, AV_PIX_FMT_NV12, WIDTH_OUT, HEIGHT_OUT);

    ASSERT_EQ (outFrame->format, AV_PIX_FMT_NV12);
    ASSERT_EQ (outFrame->width, WIDTH_OUT);
    ASSERT_EQ (outFrame->height, HEIGHT_OUT);
    ASSERT_EQ (outFrame->pts, inputFrame->pts);
}

TEST (ScalerTest, test_NV12_to_RGB24)
{
    Scaler scaler;
    Frame inputFrame (AV_PIX_FMT_NV12, WIDTH_IN, HEIGHT_IN);
    Frame outFrame
        = scaler.scale (inputFrame, AV_PIX_FMT_RGB24, WIDTH_OUT, HEIGHT_OUT);

    ASSERT_EQ (outFrame->format, AV_PIX_FMT_RGB24);
    ASSERT_EQ (outFrame->width, WIDTH_OUT);
    ASSERT_EQ (outFrame->height, HEIGHT_OUT);
    ASSERT_EQ (outFrame->pts, inputFrame->pts);
}
