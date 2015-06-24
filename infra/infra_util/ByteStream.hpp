#ifndef INFRA_BYTE_STREAM_HPP
#define INFRA_BYTE_STREAM_HPP

#include "infra_util/MemoryStream.hpp"
#include <cstdint>

namespace infra
{

    typedef MemoryStream<uint8_t> ByteStream;
    typedef MemoryStream<const uint8_t> ConstByteStream;

}

#endif
