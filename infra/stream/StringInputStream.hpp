#ifndef INFRA_STRING_INPUT_STREAM_HPP
#define INFRA_STRING_INPUT_STREAM_HPP

#include "infra/stream/InputStream.hpp"
#include "infra/util/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class StringInputStream                                                             //TICS !OOP#013
        : private StreamReader
        , public TextInputStream
    {
    public:
        explicit StringInputStream(BoundedConstString string);
        StringInputStream(BoundedConstString string, SoftFail);

    private:
        virtual void Extract(ByteRange range) override;
        virtual uint8_t ExtractOne() override;
        virtual uint8_t Peek() override;
        virtual ConstByteRange ExtractContiguousRange(std::size_t max) override;
        virtual bool IsEmpty() const override;
        virtual std::size_t SizeAvailable() const override;

    private:
        uint32_t offset = 0;
        BoundedConstString string;
    };
}

#endif
