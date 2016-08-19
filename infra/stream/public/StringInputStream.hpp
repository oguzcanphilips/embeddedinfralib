#ifndef INFRA_STRING_STREAM_HPP
#define INFRA_STRING_STREAM_HPP

#include "infra/stream/public/InputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class StringInputStream                                             //TICS !OOP#013
        : private StreamReader
        , public TextInputStream
    {
    public:
        explicit StringInputStream(const BoundedConstString& string);
        StringInputStream(const BoundedConstString& string, SoftFail);

    private:
        virtual void Extract(ByteRange range) override;
        virtual uint8_t ExtractOne() override;
        virtual uint8_t Peek() override;
        virtual void Forward(std::size_t amount) override;
        virtual bool Empty() const override;
    private:
        uint32_t offset = 0;
        const BoundedConstString& string;
    };
}

#endif
