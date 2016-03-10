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
        ByteInputStream(ConstByteRange range);
        void Reset();
    private:
        void Extract(ByteRange range) override;
        void Extract(uint8_t& element) override;
        void Peek(uint8_t& element) override;
        void Forward(std::size_t amount) override;
        bool Empty() const override;

    private:
        ConstByteRange range;
        std::size_t offset = 0;
    };
}

#endif
