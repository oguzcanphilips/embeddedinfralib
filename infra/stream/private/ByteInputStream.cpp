#include "infra/stream/public/ByteInputStream.hpp"

namespace infra
{
    ByteInputStream::ByteInputStream(ConstByteRange range)
        : DataInputStream(static_cast<StreamReader&>(*this))
        , range(range)
    {}

    ConstByteRange ByteInputStream::Processed() const
    {
        return MakeRange(range.begin(), range.begin() + offset);
    }

    ConstByteRange ByteInputStream::Remaining() const
    {
        return MakeRange(range.begin() + offset, range.end());
    }

    void ByteInputStream::Extract(ByteRange dataRange)
    {
        ReportResult(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
        offset += dataRange.size();
    }

    uint8_t ByteInputStream::ExtractOne()
    {
        uint8_t element;
        Extract(MakeByteRange(element));
        return element;
    }

    uint8_t ByteInputStream::Peek()
    {
        uint8_t element;
        ByteRange dataRange(MakeByteRange(element));
        ReportResult(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
        return element;
    }

    ConstByteRange ByteInputStream::ExtractContiguousRange(std::size_t max)
    {
        ConstByteRange result = range;
        result.pop_front(offset);
        result.shrink_from_back_to(max);
        offset += result.size();
        return result;
    }

    bool ByteInputStream::IsEmpty() const
    {
        return offset == range.size();
    }

    std::size_t ByteInputStream::SizeAvailable() const
    {
        return range.size() - offset;
    }
}
