#include "infra/stream/StringInputStream.hpp"

namespace infra
{
    StringInputStreamReader::StringInputStreamReader(BoundedConstString string)
        : string(string)
    {}

    StringInputStreamReader::StringInputStreamReader(BoundedConstString string, SoftFail)
        : StreamReader(infra::softFail)
        , string(string)
    {}

    void StringInputStreamReader::Extract(ByteRange range)
    {
        ReportResult(offset + range.size() <= string.size());
        range.shrink_from_back_to(string.size() - offset);
        std::copy(string.begin() + offset, string.begin() + offset + range.size(), range.begin());
        offset += range.size();
    }

    uint8_t StringInputStreamReader::ExtractOne()
    {
        uint8_t element = Peek();

        if (offset < string.size())
            ++offset;

        return element;
    }

    uint8_t StringInputStreamReader::Peek()
    {
        if (offset == string.size())
        {
            ReportResult(false);
            return 0;
        }
        else
        {
            ReportResult(true);
            return static_cast<uint8_t>(string.begin()[offset]);
        }
    }

    ConstByteRange StringInputStreamReader::ExtractContiguousRange(std::size_t max)
    {
        ConstByteRange result(reinterpret_cast<const uint8_t*>(string.data()) + offset, reinterpret_cast<const uint8_t*>(string.data()) + string.size());
        result.shrink_from_back_to(max);
        offset += result.size();
        return result;
    }

    bool StringInputStreamReader::Empty() const
    {
        return offset == string.size();
    }

    std::size_t StringInputStreamReader::Available() const
    {
        return string.size() - offset;
    }
}
