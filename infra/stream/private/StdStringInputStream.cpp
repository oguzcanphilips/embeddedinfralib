#include "infra/stream/public/StdStringInputStream.hpp"

namespace infra
{
    StdStringInputStream::StdStringInputStream(std::string& string)
        : TextInputStream(static_cast<StreamReader&>(*this))
        , string(string)
    {}

    StdStringInputStream::StdStringInputStream(std::string& string, SoftFail)
        : StreamReader(infra::softFail)
        , TextInputStream(static_cast<StreamReader&>(*this), infra::softFail)
        , string(string)
    {}

    void StdStringInputStream::Extract(ByteRange range)
    {
        Reader().ReportResult(range.size() <= string.size());
        range.shrink_from_back_to(string.size());
        std::copy(string.begin(), string.begin() + range.size(), range.begin());
        string.erase(0, range.size());
    }

    void StdStringInputStream::Extract(uint8_t& element)
    {
        Peek(element);

        if (!string.empty())
            string.erase(0, 1);
    }

    void StdStringInputStream::Peek(uint8_t& element)
    {
        Reader().ReportResult(!string.empty());

        if (string.empty())
            element = 0;
        else
            element = string.front();
    }

    void StdStringInputStream::Forward(std::size_t amount)
    {
        Reader().ReportResult(amount <= string.size());
        amount = std::min(amount, string.size());
        string.erase(0, amount);
    }

    bool StdStringInputStream::Empty() const
    {
        return string.empty();
    }
}
