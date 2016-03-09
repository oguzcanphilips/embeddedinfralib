#ifndef INFRA_BYTE_STREAM_HPP
#define INFRA_BYTE_STREAM_HPP

#include "infra_util/MemoryStream.hpp"
#include <cstdint>

namespace infra
{

    typedef MemoryInputStream<uint8_t> ByteInputStream;
    typedef ByteOutputStream<uint8_t> ByteOutputStream;

}

#endif
