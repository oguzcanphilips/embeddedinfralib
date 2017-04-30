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

        ConstByteRange Processed() const;   // Invariant: Processed() ++ Remaining() == range
        ConstByteRange Remaining() const;

        void Reset();

    private:
        virtual void Extract(ByteRange range) override;
        virtual uint8_t ExtractOne() override;
        virtual uint8_t Peek() override;
        virtual bool Empty() const override;

    private:
        ConstByteRange range;
        std::size_t offset = 0;
    };
}

#endif
