#ifndef HEX_COMPILER_HEX_OUTPUT_HPP
#define HEX_COMPILER_HEX_OUTPUT_HPP

#include "packs/hex_compiler/application/public/SparseVector.hpp"

namespace application
{
    std::vector<std::string> HexOutput(const SparseVector<uint8_t>& data, uint32_t maxBytesPerLine = 16);
}

#endif
