#include "Packet.hpp"

using namespace FFmpeg;

Packet::Packet ()
{
    packet
        = std::shared_ptr<AVPacket> (av_packet_alloc (), [] (AVPacket *packet)
                                     { av_packet_free (&packet); });
}

Packet::Packet (size_t size) : Packet ()
{
    int ret = av_new_packet (packet.get (), size);
    checkError (ret, "av_new_packet");
}
