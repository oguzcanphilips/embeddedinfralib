#include "infra/stream/public/StringInputStream.hpp"

namespace infra
{
    StringInputStream::StringInputStream(BoundedConstString string)
        : TextInputStream(static_cast<StreamReader&>(*this))
        , string(string)
    {}

    StringInputStream::StringInputStream(BoundedConstString string, SoftFail)
        : StreamReader(infra::softFail)
        , TextInputStream(static_cast<StreamReader&>(*this))
        , string(string)
    {}

    void StringInputStream::Extract(ByteRange range)
    {
        Reader().ReportResult(offset + range.size() <= string.size());
        range.shrink_from_back_to(string.size() - offset);
        std::copy(string.begin() + offset, string.begin() + offset + range.size(), range.begin());
        offset += range.size();
    }

    uint8_t StringInputStream::ExtractOne()
    {
        uint8_t element = Peek();

        if (offset < string.size())
            ++offset;

        return element;
    }

    uint8_t StringInputStream::Peek()
    {
        if (offset == string.size())
        {
            Reader().ReportResult(false);
            return 0;
        }
        else
        {
            Reader().ReportResult(true);
            return static_cast<uint8_t>(string.begin()[offset]);
        }
    }

    ConstByteRange StringInputStream::ExtractContiguousRange(std::size_t max)
    {
        ConstByteRange result(reinterpret_cast<const uint8_t*>(string.data()) + offset, reinterpret_cast<const uint8_t*>(string.data()) + string.size());
        result.shrink_from_back_to(max);
        offset += result.size();
        return result;
    }

    bool StringInputStream::IsEmpty() const
    {
        return offset == string.size();
    }

    std::size_t StringInputStream::SizeAvailable() const
    {
        return string.size() - offset;
    }
}
