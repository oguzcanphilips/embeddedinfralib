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

    StringOutputStream::StringOutputStream(BoundedString& string, NoFail)
        : StreamWriter(infra::noFail)
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

    const uint8_t* StringOutputStream::ConstructSaveMarker() const
    {
        return reinterpret_cast<const uint8_t*>(string.end());
    }

    std::size_t StringOutputStream::GetProcessedBytesSince(const uint8_t* marker) const
    {
        return std::distance(reinterpret_cast<const char*>(marker), string.cend());
    }

    infra::ByteRange StringOutputStream::SaveState(const uint8_t* marker)
    {
        char* copyBegin = string.begin() + std::distance(string.cbegin(), reinterpret_cast<const char*>(marker));
        char* copyEnd = string.end();
        string.resize(string.max_size());
        std::copy_backward(copyBegin, copyEnd, string.end());

        return infra::ByteRange(reinterpret_cast<uint8_t*>(copyBegin), reinterpret_cast<uint8_t*>(string.end() - std::distance(copyBegin, copyEnd)));
    }

    void StringOutputStream::RestoreState(infra::ByteRange range)
    {
        std::copy(reinterpret_cast<char*>(range.end()), string.end(), reinterpret_cast<char*>(range.begin()));
        string.resize(string.size() - range.size());
    }
}

