#include "infra/stream/SavedMarkerStream.hpp"

namespace infra
{
    SavedMarkerTextStream::SavedMarkerTextStream(TextOutputStream stream, const uint8_t* marker)
        : TextOutputStream(static_cast<StreamWriter&>(*this))
        , stream(stream)
    {
        streamRange = stream.Writer().SaveState(marker);
    }

    SavedMarkerTextStream::~SavedMarkerTextStream()
    {
        stream.Writer().RestoreState(streamRange);
    }

    void SavedMarkerTextStream::Insert(ConstByteRange range)
    {
        std::copy(range.begin(), range.end(), streamRange.begin());
        streamRange.pop_front(range.size());
    }

    void SavedMarkerTextStream::Insert(uint8_t element)
    {
        streamRange.front() = element;
        streamRange.pop_front();
    }

    const uint8_t* SavedMarkerTextStream::ConstructSaveMarker() const
    {
        return streamRange.begin();
    }

    std::size_t SavedMarkerTextStream::Available() const
    {
        return stream.Available();
    }

    std::size_t SavedMarkerTextStream::GetProcessedBytesSince(const uint8_t* marker) const
    {
        return static_cast<std::size_t>(std::distance(marker, streamRange.cbegin()));
    }

    infra::ByteRange SavedMarkerTextStream::SaveState(const uint8_t* marker)
    {
        uint8_t* copyBegin = const_cast<uint8_t*>(marker);                                                              //TICS !CON#002
        uint8_t* copyEnd = streamRange.begin();
        std::copy_backward(copyBegin, copyEnd, streamRange.end());

        return infra::ByteRange(copyBegin, streamRange.end() - std::distance(copyBegin, copyEnd));
    }

    void SavedMarkerTextStream::RestoreState(infra::ByteRange range)
    {
        std::copy(range.end(), streamRange.end(), range.begin());
        streamRange.pop_front(std::distance(streamRange.begin(), range.begin()) + std::distance(range.end(), streamRange.end()));
    }

    SavedMarkerDataStream::SavedMarkerDataStream(DataOutputStream stream, const uint8_t* marker)
        : DataOutputStream(static_cast<StreamWriter&>(*this))
        , stream(stream)
    {
        streamRange = stream.Writer().SaveState(marker);
    }

    SavedMarkerDataStream::~SavedMarkerDataStream()
    {
        stream.Writer().RestoreState(streamRange);
    }

    void SavedMarkerDataStream::Insert(ConstByteRange range)
    {
        std::copy(range.begin(), range.end(), streamRange.begin());
        streamRange.pop_front(range.size());
    }

    void SavedMarkerDataStream::Insert(uint8_t element)
    {
        streamRange.front() = element;
        streamRange.pop_front();
    }

    size_t SavedMarkerDataStream::Available() const
    {
        return stream.Available();
    }

    const uint8_t* SavedMarkerDataStream::ConstructSaveMarker() const
    {
        return streamRange.begin();
    }

    std::size_t SavedMarkerDataStream::GetProcessedBytesSince(const uint8_t* marker) const
    {
        return static_cast<std::size_t>(std::distance(marker, streamRange.cbegin()));
    }

    infra::ByteRange SavedMarkerDataStream::SaveState(const uint8_t* marker)
    {
        uint8_t* copyBegin = const_cast<uint8_t*>(marker);                                                              //TICS !CON#002
        uint8_t* copyEnd = streamRange.begin();
        std::copy_backward(copyBegin, copyEnd, streamRange.end());

        return infra::ByteRange(copyBegin, streamRange.end() - std::distance(copyBegin, copyEnd));
    }

    void SavedMarkerDataStream::RestoreState(infra::ByteRange range)
    {
        std::copy(range.end(), streamRange.end(), range.begin());
        streamRange.pop_front(std::distance(streamRange.begin(), range.begin()) + std::distance(range.end(), streamRange.end()));
    }
}
