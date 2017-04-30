#include "infra/stream/public/ByteOutputStream.hpp"

namespace infra
{
    ByteOutputStream::ByteOutputStream(ByteRange streamRange)
        : DataOutputStream(static_cast<StreamWriter&>(*this))
        , streamRange(streamRange)
    {}

    ByteOutputStream::ByteOutputStream(ByteRange streamRange, SoftFail)
        : StreamWriter(infra::softFail)
        , DataOutputStream(static_cast<StreamWriter&>(*this))
        , streamRange(streamRange)
    {}

    ByteOutputStream::ByteOutputStream(ByteRange streamRange, NoFail)
        : StreamWriter(infra::noFail)
        , DataOutputStream(static_cast<StreamWriter&>(*this))
        , streamRange(streamRange)
    {}

    ByteRange ByteOutputStream::Processed() const
    {
        return MakeRange(streamRange.begin(), streamRange.begin() + offset);
    }

    ByteRange ByteOutputStream::Remaining() const
    {
        return MakeRange(streamRange.begin() + offset, streamRange.end());
    }

    void ByteOutputStream::Reset()
    {
        offset = 0;
    }

    void ByteOutputStream::Insert(ConstByteRange dataRange)
    {
        ReportResult(dataRange.size() <= streamRange.size() - offset);
        std::copy(dataRange.begin(), dataRange.begin() + dataRange.size(), streamRange.begin() + offset);
        offset += dataRange.size();
    }

    void ByteOutputStream::Insert(uint8_t element)
    {
        ReportResult(streamRange.size() - offset > 0);
        *(streamRange.begin() + offset) = element;
        ++offset;
    }

    const uint8_t* ByteOutputStream::ConstructSaveMarker() const
    {
        return streamRange.begin() + offset;
    }

    std::size_t ByteOutputStream::GetProcessedBytesSince(const uint8_t* marker) const
    {
        return std::distance(marker, streamRange.cbegin() + offset);
    }

    infra::ByteRange ByteOutputStream::SaveState(const uint8_t* marker)
    {
        uint8_t* copyBegin = const_cast<uint8_t*>(marker);                                                              //TICS !CON#002
        uint8_t* copyEnd = streamRange.begin() + offset;
        std::copy_backward(copyBegin, copyEnd, streamRange.end());

        return infra::ByteRange(copyBegin, streamRange.end() - std::distance(copyBegin, copyEnd));
    }

    void ByteOutputStream::RestoreState(infra::ByteRange range)
    {
        std::copy(range.end(), streamRange.end(), range.begin());
        offset += std::distance(streamRange.begin() + offset, range.begin()) + std::distance(range.end(), streamRange.end());
    }
}
