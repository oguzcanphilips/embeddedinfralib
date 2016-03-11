#ifndef INFRA_STRING_STREAM_HPP
#define INFRA_STRING_STREAM_HPP

#include "infra/stream/public/InputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class StringInputStream
        : private StreamReader
        , public TextInputStream
    {
    public:
        explicit StringInputStream(const BoundedConstString& string);
        StringInputStream(const BoundedConstString& string, SoftFail);
    private:
        void Extract(ByteRange range) override;
        void Extract(uint8_t& element) override;
        void Peek(uint8_t& element) override;
        void Forward(std::size_t amount) override;
        bool Empty() const override;
    private:
        uint32_t offset = 0;
        const BoundedConstString& string;
    };
}

#endif
