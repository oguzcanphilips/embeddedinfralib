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
    class ByteOutputStream
        : private OutputStreamWriter
        , public DataOutputStream
    {
    public:
        ByteOutputStream(ByteRange range);

        ByteRange Processed() const;   // Invariant: Processed() ++ Remaining() == range
        ByteRange Remaining() const;

        void Reset();

        template<std::size_t Size>
            using WithStorage = infra::WithStorage<ByteOutputStream, std::array<uint8_t, Size>>;

    private:
        void Insert(ConstByteRange range) override;
        void Insert(uint8_t element) override;
        void Forward(std::size_t amount) override;

    private:
        ByteRange range;
        std::size_t offset = 0;
    };
}

#endif
