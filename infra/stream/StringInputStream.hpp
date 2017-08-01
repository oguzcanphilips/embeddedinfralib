#ifndef INFRA_STRING_INPUT_STREAM_HPP
#define INFRA_STRING_INPUT_STREAM_HPP

#include "infra/stream/InputStream.hpp"
#include "infra/util/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class StringInputStreamReader
        : public StreamReader
    {
    public:
        explicit StringInputStreamReader(BoundedConstString string);
        StringInputStreamReader(BoundedConstString string, SoftFail);

    private:
        virtual void Extract(ByteRange range) override;
        virtual uint8_t ExtractOne() override;
        virtual uint8_t Peek() override;
        virtual ConstByteRange ExtractContiguousRange(std::size_t max) override;
        virtual bool Empty() const override;
        virtual std::size_t Available() const override;

    private:
        uint32_t offset = 0;
        BoundedConstString string;
    };

    class StringInputStream
        : public TextInputStream::WithReader<StringInputStreamReader>
    {
    public:
        template<std::size_t Max>
            using WithStorage = infra::WithStorage<TextInputStream::WithReader<StringInputStreamReader>, infra::BoundedString::WithStorage<Max>>;

        using TextInputStream::WithReader<StringInputStreamReader>::WithReader;
    };
}

#endif
