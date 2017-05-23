#ifndef INFRA_BYTE_INPUT_STREAM_HPP
#define INFRA_BYTE_INPUT_STREAM_HPP

#include "infra/stream/public/InputStream.hpp"

namespace infra
{
    class ByteInputStream                                                                                   //TICS !OOP#013
        : private StreamReader
        , public DataInputStream
    {
    public:
        explicit ByteInputStream(ConstByteRange range);
        ByteInputStream(ConstByteRange range, SoftFail);

        ConstByteRange Processed() const;   // Invariant: Processed() ++ Remaining() == range
        ConstByteRange Remaining() const;

    private:
        virtual void Extract(ByteRange range) override;
        virtual uint8_t ExtractOne() override;
        virtual uint8_t Peek() override;
        virtual ConstByteRange ExtractContiguousRange(std::size_t max) override;
        virtual bool IsEmpty() const override;
        virtual std::size_t SizeAvailable() const override;

    private:
        ConstByteRange range;
        std::size_t offset = 0;
    };
}

#endif
