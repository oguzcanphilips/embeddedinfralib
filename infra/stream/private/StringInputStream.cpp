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

    bool StringInputStream::Empty() const
    {
        return offset == string.size();
    }
}
