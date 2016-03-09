#include "infra/stream/public/StringStream.hpp"

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

    StringOutputStream::StringOutputStream(BoundedString& string)
        : TextOutputStreamHelper<char>(static_cast<OutputStream<char>&>(*this))
        , string(string)
    {}

    StringOutputStream::StringOutputStream(BoundedString& string, SoftFail)
        : TextOutputStreamHelper<char>(static_cast<OutputStream<char>&>(*this))
        , string(string)
        , softFail(true)
    {}

    StringOutputStream::~StringOutputStream()
    {
        assert(checkedFail);
    }

    void StringOutputStream::Insert(MemoryRange<const char> range)
    {
        if (softFail)
        {
            checkedFail = false;
            std::size_t spaceLeft = string.max_size() - string.size();
            if (range.size() > spaceLeft)
            {
                failed = true;
                range.shrink_from_back_to(spaceLeft);
            }
        }
        
        string.append(range.begin(), range.size());
    }

    void StringOutputStream::Insert(char element)
    {
        if (softFail)
        {
            checkedFail = false;
            if (!string.full())
                string.push_back(element);
            else
                failed = true;
        }
        else
            string.push_back(element);
    }

    void StringOutputStream::Forward(std::size_t amount)
    {
        string.append(amount, ' ');
    }

    bool StringOutputStream::HasFailed() const
    {
        checkedFail = true;
        return failed;
    }

    void StringOutputStream::ResetFail()
    {
        assert(softFail);
        assert(checkedFail);
        failed = false;
    }
}

