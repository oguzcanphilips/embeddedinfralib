#include "infra/stream/public/StdStringInputStream.hpp"

namespace infra
{
    StdStringInputStream::StdStringInputStream(const std::string& string)
        : TextInputStream(static_cast<StreamReader&>(*this))
        , string(string)
    {}

    StdStringInputStream::StdStringInputStream(const std::string& string, SoftFail)
        : StreamReader(infra::softFail)
        , TextInputStream(static_cast<StreamReader&>(*this))
        , string(string)
    {}

    void StdStringInputStream::Extract(ByteRange range)
    {
        Reader().ReportResult(offset + range.size() <= string.size());
        range.shrink_from_back_to(string.size() - offset);
        std::copy(string.begin() + offset, string.begin() + offset + range.size(), range.begin());
        offset += range.size();
    }

    uint8_t StdStringInputStream::ExtractOne()
    {
        uint8_t element = Peek();

        if (offset < string.size())
            ++offset;

        return element;
    }

    uint8_t StdStringInputStream::Peek()
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

    ConstByteRange StdStringInputStream::ExtractContiguousRange(std::size_t max)
    {
        ConstByteRange result(reinterpret_cast<const uint8_t*>(string.data()) + offset, reinterpret_cast<const uint8_t*>(string.data()) + string.size());
        result.shrink_from_back_to(max);
        offset += result.size();
        return result;
    }

    bool StdStringInputStream::IsEmpty() const
    {
        return offset == string.size();
    }

    std::size_t StdStringInputStream::SizeAvailable() const
    {
        return string.size() - offset;
    }
}
