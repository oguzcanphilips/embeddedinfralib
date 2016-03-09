#include "infra/stream/public/MemoryStream.hpp"

namespace infra
{
    ByteOutputStream::ByteOutputStream(ByteRange range)
        : DataOutputStream(static_cast<OutputStreamWriter&>(*this))
        , range(range)
    {}

    ByteRange ByteOutputStream::Processed() const
    {
        return MakeRange(range.begin(), range.begin() + offset);
    }

    ByteRange ByteOutputStream::Remaining() const
    {
        return MakeRange(range.begin() + offset, range.end());
    }

    void ByteOutputStream::Reset()
    {
        offset = 0;
    }

    void ByteOutputStream::Insert(ConstByteRange dataRange)
    {
        assert(dataRange.size() <= range.size() - offset);
        std::copy(dataRange.begin(), dataRange.begin() + dataRange.size(), range.begin() + offset);
        offset += dataRange.size();
    }

    void ByteOutputStream::Insert(uint8_t element)
    {
        assert(range.size() - offset > 0);
        *(range.begin() + offset) = element;
        ++offset;
    }

    void ByteOutputStream::Forward(std::size_t amount)
    {
        assert(amount <= range.size() - offset);
        offset += amount;
    }
}
