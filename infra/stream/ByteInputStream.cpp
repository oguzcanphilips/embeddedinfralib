#include "infra/stream/ByteInputStream.hpp"

namespace infra
{
    ByteInputStreamReader::ByteInputStreamReader(ConstByteRange range)
        : range(range)
    {}

    ByteInputStreamReader::ByteInputStreamReader(ConstByteRange range, SoftFail)
        : StreamReader(infra::softFail)
        , range(range)
    {}

    ConstByteRange ByteInputStreamReader::Processed() const
    {
        return MakeRange(range.begin(), range.begin() + offset);
    }

    ConstByteRange ByteInputStreamReader::Remaining() const
    {
        return MakeRange(range.begin() + offset, range.end());
    }

    void ByteInputStreamReader::Extract(ByteRange dataRange)
    {
        ReportResult(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
        offset += dataRange.size();
    }

    uint8_t ByteInputStreamReader::ExtractOne()
    {
        uint8_t element;
        Extract(MakeByteRange(element));
        return element;
    }

    uint8_t ByteInputStreamReader::Peek()
    {
        uint8_t element;
        ByteRange dataRange(MakeByteRange(element));
        ReportResult(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
        return element;
    }

    ConstByteRange ByteInputStreamReader::ExtractContiguousRange(std::size_t max)
    {
        ConstByteRange result = range;
        result.pop_front(offset);
        result.shrink_from_back_to(max);
        offset += result.size();
        return result;
    }

    bool ByteInputStreamReader::Empty() const
    {
        return offset == range.size();
    }

    std::size_t ByteInputStreamReader::Available() const
    {
        return range.size() - offset;
    }
}
