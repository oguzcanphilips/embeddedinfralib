#include "infra/stream/StdStringOutputStream.hpp"

namespace infra
{
    StdStringOutputStreamWriter::StdStringOutputStreamWriter(std::string& string)
        : string(string)
    {}

    StdStringOutputStreamWriter::StdStringOutputStreamWriter(std::string& string, SoftFail)
        : StreamWriter(infra::softFail)
        , string(string)
    {}

    StdStringOutputStreamWriter::StdStringOutputStreamWriter(std::string& string, NoFail)
        : StreamWriter(infra::noFail)
        , string(string)
    {}

    void StdStringOutputStreamWriter::Insert(ConstByteRange range)
    {
        string.append(reinterpret_cast<const char*>(range.begin()), range.size());
    }

    void StdStringOutputStreamWriter::Insert(uint8_t element)
    {
        string.push_back(static_cast<char>(element));
    }
}
