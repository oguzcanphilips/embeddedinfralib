#include "infra/stream/public/StringOutputStream.hpp"

namespace infra
{
    StringOutputStream::StringOutputStream(BoundedString& string)
        : TextOutputStream(static_cast<StreamWriter&>(*this))
        , string(string)
        , softFail(false)
    {}

    StringOutputStream::StringOutputStream(BoundedString& string, SoftFail)
        : TextOutputStream(static_cast<StreamWriter&>(*this))
        , string(string)
        , softFail(true)
    {}

    StringOutputStream::~StringOutputStream()
    {
        assert(checkedFail);
    }

    void StringOutputStream::Insert(ConstByteRange range)
    {
        std::size_t spaceLeft = string.max_size() - string.size();
        if (range.size() > spaceLeft)
        {
            failed = true;
            range.shrink_from_back_to(spaceLeft);            
            assert(softFail);
        }
        checkedFail = !softFail;
        string.append(reinterpret_cast<const char*>(range.begin()), range.size());
    }

    void StringOutputStream::Insert(uint8_t element)
    {
        if (!string.full())
            string.push_back(static_cast<uint8_t>(element));
        else
        {
            failed = true;
            assert(softFail);
        }
        checkedFail = !softFail;
    }

    void StringOutputStream::Forward(std::size_t amount)
    {
        std::size_t spaceLeft = string.max_size() - string.size();
        if (amount > spaceLeft)
        {
            failed = true;
            amount = spaceLeft;
            assert(softFail);
        }
        checkedFail = !softFail;
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

