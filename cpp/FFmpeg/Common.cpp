#include "Common.hpp"

namespace FFmpeg
{

    void checkError (int ret, const std::string &message)
    {
        if (ret >= 0)
        {
            return;
        }
        constexpr int length = 256;
        std::vector<char> err (length);
        av_strerror (ret, err.data (), length);
        throw std::runtime_error (message + " failed: " + err.data ());
    }

    auto currentPts (const AVRational &time_base) -> int64_t
    {
        static auto globalBaseTime = std::chrono::steady_clock::now ();
        auto now = std::chrono::steady_clock::now ();
        double seconds
            = std::chrono::duration<double> (now - globalBaseTime).count ();
        return seconds * time_base.den / time_base.num;
    }
} // namespace FFmpeg
