#include "infra/stream/IoOutputStream.hpp"
#include <cstdlib>
#include <iostream>
#include <limits>

namespace infra
{
    void IoOutputStreamWriter::Insert(ConstByteRange range)
    {
        std::string s(range.begin(), range.end());
        s.push_back(0);
        std::cout << s << std::flush;
    }

    void IoOutputStreamWriter::Insert(uint8_t element)
    {
        std::cout << static_cast<char>(element);
    }

    std::size_t IoOutputStreamWriter::Available() const
    {
        return std::numeric_limits<std::size_t>::max();
    }

    const uint8_t* IoOutputStreamWriter::ConstructSaveMarker() const
    {
        std::abort();
    }

    std::size_t IoOutputStreamWriter::GetProcessedBytesSince(const uint8_t* marker) const
    {
        std::abort();
    }

    infra::ByteRange IoOutputStreamWriter::SaveState(const uint8_t* marker)
    {
        std::abort();
    }

    void IoOutputStreamWriter::RestoreState(infra::ByteRange range)
    {
        std::abort();
    }
}

