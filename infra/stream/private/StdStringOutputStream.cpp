#include "infra/stream/public//StdStringOutputStream.hpp"

namespace infra
{
    StdStringOutputStream::StdStringOutputStream(std::string& string)
        : TextOutputStream(static_cast<StreamWriter&>(*this))
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

    void StdStringOutputStream::Forward(std::size_t amount)
    {
        string.append(amount, ' ');
    }

    bool StdStringOutputStream::HasFailed() const
    {
        return false;
    }

    void StdStringOutputStream::ResetFail()
    {}
}
