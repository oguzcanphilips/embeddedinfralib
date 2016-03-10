#include "infra/stream/public//StdStringInputStream.hpp"

namespace infra
{
    StdStringInputStream::StdStringInputStream(std::string& string)
        : TextInputStreamHelper<char>(static_cast<InputStream<char>&>(*this))
        , string(string)
    {}

    StdStringInputStream::StdStringInputStream(std::string& string, SoftFail)
        : TextInputStreamHelper<char>(static_cast<InputStream<char>&>(*this), softFail)
        , string(string)
    {}

    void StdStringInputStream::Extract(MemoryRange<char> range)
    {
        ReportFailureCheck(range.size() > string.size());
        range.shrink_from_back_to(string.size());
        std::copy(string.begin(), string.begin() + range.size(), range.begin());
        string.erase(0, range.size());
    }

    void StdStringInputStream::Extract(char& element)
    {
        Peek(element);

        if (!string.empty())
            string.erase(0, 1);
    }

    void StdStringInputStream::Peek(char& element)
    {
        ReportFailureCheck(string.empty());

        if (string.empty())
            element = 0;
        else
            element = string.front();
    }

    void StdStringInputStream::Forward(std::size_t amount)
    {
        ReportFailureCheck(amount > string.size());
        amount = std::min(amount, string.size());
        string.erase(0, amount);
    }

    bool StdStringInputStream::Empty() const
    {
        return string.empty();
    }

    void StdStringInputStream::ReportFailureCheck(bool hasCheckFailed)
    {
        InputStream::ReportFailureCheck(hasCheckFailed);
    }

    bool StdStringInputStream::HasFailed() const
    {
        return InputStream::HasFailed();
    }

    void StdStringInputStream::ResetFail()
    {
        InputStream::ResetFail();
    }
}
