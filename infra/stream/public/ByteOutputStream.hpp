#ifndef INFRA_BYTE_OUTPUT_STREAM_HPP
#define INFRA_BYTE_OUTPUT_STREAM_HPP

// With a ByteOutputStream, you can easily place all sorts of objects into a block of memory.
// A ByteOutputStream is created with a MemoryRange as argument, objects streamed out of the
// ByteOutputStream are taken from that range.
//
// Example:
//
// uint16_t myData = 0x1234;
// uint16_t myChecksum = 0x5678;
//
// std::array<uint8_t, 20> memory;
// infra::ByteOutputStream writeStream(memory);
// writeStream << uint8_t(4) << myData << myChecksum;
//
// Now memory contains the bytes 0x04, 0x12, 0x34, 0x56, 0x78.

#include "infra/stream/public/OutputStream.hpp"
#include "infra/util/public/WithStorage.hpp"

namespace infra
{
    class ByteOutputStream                                                                                      //TICS !OOP#013
        : private StreamWriter
        , public DataOutputStream
    {
    public:
        template<std::size_t Size>
            using WithStorage = infra::WithStorage<ByteOutputStream, std::array<uint8_t, Size>>;

        explicit ByteOutputStream(ByteRange range);
        ByteOutputStream(ByteRange range, SoftFail);
        ByteOutputStream(ByteRange range, NoFail);

        ByteRange Processed() const;   // Invariant: Processed() ++ Remaining() == range
        ByteRange Remaining() const;

        void Reset();

        template<class T>
            ReservedProxy<T> Reserve();

    private:
        virtual void Insert(ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;

        virtual const uint8_t* ConstructSaveMarker() const override;
        virtual std::size_t GetProcessedBytesSince(const uint8_t* marker) const override;
        virtual infra::ByteRange SaveState(const uint8_t* marker) override;
        virtual void RestoreState(infra::ByteRange range) override;

    private:
        ByteRange streamRange;
        std::size_t offset = 0;
    };

    ////    Implementation    ////

    template<class T>
    ReservedProxy<T> ByteOutputStream::Reserve()
    {
        ByteRange reservedRange(streamRange.begin() + offset, streamRange.begin() + offset + sizeof(T));
        std::size_t spaceLeft = streamRange.size() - offset;
        bool spaceOk = reservedRange.size() <= spaceLeft;
        ReportResult(spaceOk);
        if (!spaceOk)
            reservedRange.shrink_from_back_to(spaceLeft);

        offset += reservedRange.size();

        return ReservedProxy<T>(reservedRange);
    }
}

#endif
