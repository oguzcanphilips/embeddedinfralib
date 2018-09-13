#include "infra/stream/ByteOutputStream.hpp"

namespace infra
{
    ByteOutputStreamWriter::ByteOutputStreamWriter(ByteRange streamRange)
        : streamRange(streamRange)
    {}

    ByteRange ByteOutputStreamWriter::Processed() const
    {
        return MakeRange(streamRange.begin(), streamRange.begin() + offset);
    }

    ByteRange ByteOutputStreamWriter::Remaining() const
    {
        return MakeRange(streamRange.begin() + offset, streamRange.end());
    }

    void ByteOutputStreamWriter::Reset()
    {
        offset = 0;
    }

    void ByteOutputStreamWriter::Insert(ConstByteRange dataRange, StreamErrorPolicy& errorPolicy)
    {
        errorPolicy.ReportResult(dataRange.size() <= streamRange.size() - offset);
        dataRange.shrink_from_back_to(streamRange.size() - offset);
        std::copy(dataRange.begin(), dataRange.begin() + dataRange.size(), streamRange.begin() + offset);
        offset += dataRange.size();
    }

    std::size_t ByteOutputStreamWriter::Available() const
    {
        return streamRange.size() - offset;
    }

    const uint8_t* ByteOutputStreamWriter::ConstructSaveMarker() const
    {
        return streamRange.begin() + offset;
    }

    std::size_t ByteOutputStreamWriter::GetProcessedBytesSince(const uint8_t* marker) const
    {
        return static_cast<std::size_t>(std::distance(marker, streamRange.cbegin() + offset));
    }

    infra::ByteRange ByteOutputStreamWriter::SaveState(const uint8_t* marker)
    {
        uint8_t* copyBegin = const_cast<uint8_t*>(marker);                                                              //TICS !CON#002
        uint8_t* copyEnd = streamRange.begin() + offset;
        std::copy_backward(copyBegin, copyEnd, streamRange.end());

        return infra::ByteRange(copyBegin, streamRange.end() - std::distance(copyBegin, copyEnd));
    }

    void ByteOutputStreamWriter::RestoreState(infra::ByteRange range)
    {
        std::copy(range.end(), streamRange.end(), range.begin());
        offset += std::distance(streamRange.begin() + offset, range.begin()) + std::distance(range.end(), streamRange.end());
    }
}
