#include "infra/stream/public/StringOutputStream.hpp"

namespace infra
{
    StringOutputStream::StringOutputStream(BoundedString& string)
        : TextOutputStream(static_cast<StreamWriter&>(*this))
        , string(string)
    {}

    StringOutputStream::StringOutputStream(BoundedString& string, SoftFail)
        : StreamWriter(infra::softFail)
        , TextOutputStream(static_cast<StreamWriter&>(*this))
        , string(string)
    {}

    void StringOutputStream::Insert(ConstByteRange range)
    {
        std::size_t spaceLeft = string.max_size() - string.size();
        bool spaceOk = range.size() <= spaceLeft;
        ReportResult(spaceOk);
        if (!spaceOk)
            range.shrink_from_back_to(spaceLeft);            
        string.append(reinterpret_cast<const char*>(range.begin()), range.size());
    }

    void StringOutputStream::Insert(uint8_t element)
    {
        bool isOk = !string.full();
        if (isOk)
            string.push_back(static_cast<char>(element));
        ReportResult(isOk);
    }

    void StringOutputStream::Forward(std::size_t amount)
    {
        std::size_t spaceLeft = string.max_size() - string.size();
        ReportResult(amount <= spaceLeft);
        if (amount > spaceLeft)
            amount = spaceLeft;
        string.append(amount, ' ');
    }
}

