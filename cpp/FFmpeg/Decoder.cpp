#include "Decoder.hpp"

using namespace FFmpeg;

Decoder::Decoder (AVCodecID codecId)
{
    std::lock_guard lock (mutex);
    const AVCodec *decoder = avcodec_find_decoder (codecId);
    if (decoder == nullptr)
    {
        throw std::runtime_error ("Could not find decoder");
    }

    ctx = avcodec_alloc_context3 (decoder);
    if (ctx == nullptr)
    {
        throw std::runtime_error ("Could not allocate AVCodecContext");
    }
    if (decoder->id == AV_CODEC_ID_OPUS)
    {
        ctx->time_base = (AVRational){ 1, OPUS_SAMPLE_RATE };
        ctx->sample_rate = OPUS_SAMPLE_RATE;
        ctx->bit_rate = OPUS_BIT_RATE;
        ctx->sample_fmt = AV_SAMPLE_FMT_FLT;
        av_channel_layout_default (&ctx->ch_layout, 2);
    }
    ctx->flags |= AV_CODEC_FLAG_LOW_DELAY;
    int ret = avcodec_open2 (ctx, decoder, nullptr) < 0;
    checkError (ret, "avcodec_open2");
}

Decoder::~Decoder ()
{
    if (ctx != nullptr)
    {
        avcodec_free_context (&ctx);
        ctx = nullptr;
    }
}

void Decoder::send (const Packet &packet)
{
    std::lock_guard lock (mutex);
    int ret = avcodec_send_packet (ctx, packet.get ());
    checkError (ret, "avcodec_send_packet");
    while (true)
    {
        Frame frame;
        int ret = avcodec_receive_frame (ctx, frame.get ());
        if (ret == AVERROR (EAGAIN) || ret == AVERROR_EOF)
        {
            break;
        }
        checkError (ret, "avcodec_receive_frame");
        buffer.push_back (frame);
    }
}

auto Decoder::receive () -> std::vector<Frame>
{
    std::lock_guard lock (mutex);
    std::vector<Frame> result = std::move (buffer);
    buffer.clear ();
    return result;
}

void Decoder::flush ()
{
    std::lock_guard lock (mutex);
    int ret = avcodec_send_packet (ctx, nullptr);
    checkError (ret, "avcodec_send_packet");
}
