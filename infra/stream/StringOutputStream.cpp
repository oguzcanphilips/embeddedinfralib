#include "infra/stream/StringOutputStream.hpp"

namespace infra
{
    StringOutputStreamWriter::StringOutputStreamWriter(BoundedString& string)
        : string(string)
    {}

    void StringOutputStreamWriter::Insert(ConstByteRange range, StreamErrorPolicy& errorPolicy)
    {
        std::size_t spaceLeft = Available();
        bool spaceOk = range.size() <= spaceLeft;
        errorPolicy.ReportResult(spaceOk);
        if (!spaceOk)
            range.shrink_from_back_to(spaceLeft);
        string.append(reinterpret_cast<const char*>(range.begin()), range.size());
    }

    std::size_t StringOutputStreamWriter::Available() const
    {
        return string.max_size() - string.size();
    }

    const uint8_t* StringOutputStreamWriter::ConstructSaveMarker() const
    {
        return reinterpret_cast<const uint8_t*>(string.end());
    }

    std::size_t StringOutputStreamWriter::GetProcessedBytesSince(const uint8_t* marker) const
    {
        return static_cast<std::size_t>(std::distance(reinterpret_cast<const char*>(marker), string.cend()));
    }

    infra::ByteRange StringOutputStreamWriter::SaveState(const uint8_t* marker)
    {
        char* copyBegin = string.begin() + std::distance(string.cbegin(), reinterpret_cast<const char*>(marker));
        char* copyEnd = string.end();
        string.resize(string.max_size());
        std::copy_backward(copyBegin, copyEnd, string.end());

        return infra::ByteRange(reinterpret_cast<uint8_t*>(copyBegin), reinterpret_cast<uint8_t*>(string.end() - std::distance(copyBegin, copyEnd)));
    }

    void StringOutputStreamWriter::RestoreState(infra::ByteRange range)
    {
        std::copy(reinterpret_cast<char*>(range.end()), string.end(), reinterpret_cast<char*>(range.begin()));
        string.resize(string.size() - range.size());
    }

    StringOutputStream::StringOutputStream(BoundedString& storage)
        : TextOutputStream::WithWriter<StringOutputStreamWriter>(storage)
    {}

    StringOutputStream::StringOutputStream(BoundedString& storage, const SoftFail&)
        : TextOutputStream::WithWriter<StringOutputStreamWriter>(storage, softFail)
    {}

    StringOutputStream::StringOutputStream(BoundedString& storage, const NoFail&)
        : TextOutputStream::WithWriter<StringOutputStreamWriter>(storage, noFail)
    {}
}
