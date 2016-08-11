#ifndef INFRA_BYTE_INPUT_STREAM_HPP
#define INFRA_BYTE_INPUT_STREAM_HPP

#include "infra/stream/public/InputStream.hpp"

namespace infra
{
    class ByteInputStream
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
        virtual void Extract(uint8_t& element) override;
        virtual void Peek(uint8_t& element) override;
        virtual void Forward(std::size_t amount) override;
        virtual bool Empty() const override;

    private:
        ConstByteRange range;
        std::size_t offset = 0;
    };
}

#endif
