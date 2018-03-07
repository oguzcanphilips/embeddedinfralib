#ifndef INFRA_BYTE_INPUT_STREAM_HPP
#define INFRA_BYTE_INPUT_STREAM_HPP

#include "infra/stream/InputStream.hpp"
#include <array>

namespace infra
{
    class ByteInputStreamReader
        : public StreamReader
    {
    public:
        explicit ByteInputStreamReader(ConstByteRange range);

        ConstByteRange Processed() const;   // Invariant: Processed() ++ Remaining() == range
        ConstByteRange Remaining() const;

    private:
        virtual void Extract(ByteRange range, StreamErrorPolicy& errorPolicy) override;
        virtual uint8_t ExtractOne(StreamErrorPolicy& errorPolicy) override;
        virtual uint8_t Peek(StreamErrorPolicy& errorPolicy) override;
        virtual ConstByteRange ExtractContiguousRange(std::size_t max) override;
        virtual bool Empty() const override;
        virtual std::size_t Available() const override;

    private:
        ConstByteRange range;
        std::size_t offset = 0;
    };

    class ByteInputStream
        : public DataInputStream::WithReader<ByteInputStreamReader>
    {
    public:
        template<std::size_t Max>
            using WithStorage = infra::WithStorage<DataInputStream::WithReader<ByteInputStreamReader>, std::array<uint8_t, Max>>;

        using DataInputStream::WithReader<ByteInputStreamReader>::WithReader;
    };
}

#endif
