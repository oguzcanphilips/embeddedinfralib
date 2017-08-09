#ifndef INFRA_LIMITED_INPUT_STREAM_HPP
#define INFRA_LIMITED_INPUT_STREAM_HPP

#include "infra/stream/InputStream.hpp"

namespace infra
{
    class LimitedStreamReader
        : public StreamReader
    {
    public:
        LimitedStreamReader(StreamReader& input, uint32_t length);

    public:
        virtual void Extract(ByteRange range) override;
        virtual uint8_t ExtractOne() override;
        virtual uint8_t Peek() override;
        virtual ConstByteRange ExtractContiguousRange(std::size_t max) override;
        virtual bool Empty() const override;
        virtual std::size_t Available() const override;

    private:
        StreamReader& input;
        uint32_t length;
    };
}

#endif