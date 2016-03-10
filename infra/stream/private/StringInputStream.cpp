#include "infra/stream/public/StringInputStream.hpp"

namespace infra
{
    StringInputStream::StringInputStream(BoundedString& string)
        : TextInputStreamHelper<char>(static_cast<InputStream<char>&>(*this))
        , string(string)
    {}

    StringInputStream::StringInputStream(BoundedString& string, SoftFail)
        : TextInputStreamHelper<char>(static_cast<InputStream<char>&>(*this), softFail)
        , string(string)
    {}

    void StringInputStream::Extract(MemoryRange<char> range)
    {
        ReportFailureCheck(range.size() > string.size());
        range.shrink_from_back_to(string.size());
        std::copy(string.begin(), string.begin() + range.size(), range.begin());
        string.erase(0, range.size());
    }

    void StringInputStream::Extract(char& element)
    {
        Peek(element);

        if (!string.empty())
            string.erase(0, 1);
    }

    void StringInputStream::Peek(char& element)
    {
        ReportFailureCheck(string.empty());
        if (string.empty())
            element = 0;
        else
            element = string.front();
    }

    void StringInputStream::Forward(std::size_t amount)
    {
        ReportFailureCheck(amount > string.size());
        amount = std::min(amount, string.size());
        string.erase(0, amount);
    }

    bool StringInputStream::Empty() const
    {
        return string.empty();
    }

    void StringInputStream::ReportFailureCheck(bool hasCheckFailed)
    {
        InputStream::ReportFailureCheck(hasCheckFailed);
    }

    bool StringInputStream::HasFailed() const
    {
        return InputStream::HasFailed();
    }

    void StringInputStream::ResetFail()
    {
        InputStream::ResetFail();
    }
}

