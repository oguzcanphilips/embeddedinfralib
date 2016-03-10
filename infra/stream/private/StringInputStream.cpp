#include "infra/stream/public/StringInputStream.hpp"

namespace infra
{
    StringInputStream::StringInputStream(BoundedString& string)
        : TextInputStream(static_cast<StreamReader&>(*this))
        , string(string)
    {}
    StringInputStream::StringInputStream(BoundedString& string, SoftFail)
        : StreamReader(infra::softFail)
        , TextInputStream(static_cast<StreamReader&>(*this), infra::softFail)
        , string(string)
    {
    }

    void StringInputStream::Extract(ByteRange range)
    {
        assert(range.size() <= string.size());
        range.shrink_from_back_to(string.size());
        std::copy(string.begin(), string.begin() + range.size(), range.begin());
        string.erase(0, range.size());
    }

    void StringInputStream::Extract(uint8_t& element)
    {
        Peek(element);

        if (!string.empty())
            string.erase(0, 1);
    }

    void StringInputStream::Peek(uint8_t& element)
    {
        if (string.empty())
            element = 0;
        else
            element = string.front();
    }

    void StringInputStream::Forward(std::size_t amount)
    {
        assert(amount <= string.size());
        amount = std::min(amount, string.size());
        string.erase(0, amount);
    }

    bool StringInputStream::Empty() const
    {
        return string.empty();
    }
}

