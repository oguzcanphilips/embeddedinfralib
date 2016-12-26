#include "infra/stream/public/ByteOutputStream.hpp"

namespace infra
{
    ByteOutputStream::ByteOutputStream(ByteRange range)
        : DataOutputStream(static_cast<StreamWriter&>(*this))
        , range(range)
    {}

    ByteOutputStream::ByteOutputStream(ByteRange range, SoftFail)
        : StreamWriter(infra::softFail)
        , DataOutputStream(static_cast<StreamWriter&>(*this))
        , range(range)
    {}

    ByteOutputStream::ByteOutputStream(ByteRange range, NoFail)
        : StreamWriter(infra::noFail)
        , DataOutputStream(static_cast<StreamWriter&>(*this))
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
        ReportResult(dataRange.size() <= range.size() - offset);
        std::copy(dataRange.begin(), dataRange.begin() + dataRange.size(), range.begin() + offset);
        offset += dataRange.size();
    }

    void ByteOutputStream::Insert(uint8_t element)
    {
        ReportResult(range.size() - offset > 0);
        *(range.begin() + offset) = element;
        ++offset;
    }

    void ByteOutputStream::Forward(std::size_t amount)
    {
        ReportResult(amount <= range.size() - offset);
        offset += amount;
        if (offset > range.size())
            offset = range.size();
    }
}
