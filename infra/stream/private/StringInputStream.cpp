#include "infra/stream/public/StringInputStream.hpp"

namespace infra
{
    StringInputStream::StringInputStream(const BoundedConstString& string)
        : TextInputStream(static_cast<StreamReader&>(*this))
        , string(string)
    {}
    StringInputStream::StringInputStream(const BoundedConstString& string, SoftFail)
        : StreamReader(infra::softFail)
        , TextInputStream(static_cast<StreamReader&>(*this), infra::softFail)
        , string(string)
    {
    }

    void StringInputStream::Extract(ByteRange range)
    {
        Reader().ReportResult(offset + range.size() <= string.size());
        range.shrink_from_back_to(string.size()-offset);
        std::copy(string.begin() + offset, string.begin() + offset + range.size(), range.begin());
        offset += range.size();
    }

    void StringInputStream::Extract(uint8_t& element)
    {
        Peek(element);
        if (offset < string.size())
            ++offset;
    }

    void StringInputStream::Peek(uint8_t& element)
    {
        if (offset == string.size())
        {
            Reader().ReportResult(false);
        }
        else
        {
            Reader().ReportResult(true);
            element = static_cast<uint8_t>(string.begin()[offset]);
        }
    }

    void StringInputStream::Forward(std::size_t amount)
    {
        offset += amount;
        if (offset > string.size())
        {
            Reader().ReportResult(false);
            offset = string.size();
        }
        else
        {
            Reader().ReportResult(true);
        }
    }

    bool StringInputStream::Empty() const
    {
        return offset == string.size();
    }
}

