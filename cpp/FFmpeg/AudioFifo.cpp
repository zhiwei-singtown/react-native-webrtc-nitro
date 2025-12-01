#include "AudioFifo.hpp"

using namespace FFmpeg;

AudioFifo::~AudioFifo ()
{
    if (fifo != nullptr)
    {
        av_audio_fifo_free (fifo);
        fifo = nullptr;
    }
}

void AudioFifo::write (const Frame &frame)
{
    std::lock_guard lock (mutex);
    if (fifo == nullptr)
    {
        // init
        format = (AVSampleFormat)frame->format;
        channels = frame->ch_layout.nb_channels;
        sample_rate = frame->sample_rate;
        pts = frame->pts;
        constexpr int AUDIO_FIFO_BUFFER_SIZE = 1024;
        fifo = av_audio_fifo_alloc (format, channels, AUDIO_FIFO_BUFFER_SIZE);
        if (fifo == nullptr)
        {
            throw std::runtime_error ("av_audio_fifo_alloc faield");
        }
    }

    int ret
        = av_audio_fifo_write (fifo, (void **)frame->data, frame->nb_samples);
    checkError (ret, "av_audio_fifo_write");
    if (ret < frame->nb_samples)
    {
        throw std::runtime_error ("Could not write to audio fifo");
    }
}

auto AudioFifo::read (int nb_samples) -> std::optional<Frame>
{
    std::lock_guard lock (mutex);
    if (fifo == nullptr)
    {
        return std::nullopt;
    }
    if (av_audio_fifo_size (fifo) == 0)
    {
        return std::nullopt;
    }
    if (nb_samples < 0)
    {
        nb_samples = av_audio_fifo_size (fifo);
    }
    if (av_audio_fifo_size (fifo) < nb_samples)
    {
        return std::nullopt;
    }

    Frame frame (format, sample_rate, channels, nb_samples, pts);

    int ret = av_audio_fifo_read (fifo, (void **)frame->data, nb_samples);
    checkError (ret, "av_audio_fifo_read");

    pts += nb_samples;

    return frame;
}

void AudioFifo::clear ()
{
    std::lock_guard lock (mutex);
    if (fifo != nullptr)
    {
        av_audio_fifo_reset (fifo);
    }
}
