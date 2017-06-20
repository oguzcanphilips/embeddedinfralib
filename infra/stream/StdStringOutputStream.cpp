#include "infra/stream/StdStringOutputStream.hpp"

namespace infra
{
    StdStringOutputStream::StdStringOutputStream(std::string& string)
        : TextOutputStream(static_cast<StreamWriter&>(*this))
        , string(string)
    {}

    StdStringOutputStream::StdStringOutputStream(std::string& string, SoftFail)
        : StreamWriter(infra::softFail)
        , TextOutputStream(static_cast<StreamWriter&>(*this))
        , string(string)
    {}

    StdStringOutputStream::StdStringOutputStream(std::string& string, NoFail)
        : StreamWriter(infra::noFail)
        , TextOutputStream(static_cast<StreamWriter&>(*this))
        , string(string)
    {}

    StdStringOutputStream::~StdStringOutputStream()
    {}

    void StdStringOutputStream::Insert(ConstByteRange range)
    {
        string.append(reinterpret_cast<const char*>(range.begin()), range.size());
    }

    void StdStringOutputStream::Insert(uint8_t element)
    {
        string.push_back(static_cast<char>(element));
    }
}
