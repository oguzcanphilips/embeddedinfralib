#ifndef INFRA_STRING_STREAM_HPP
#define INFRA_STRING_STREAM_HPP

#include "infra/stream/public/InputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class StringInputStream
        : private InputStreamReader
        , public TextInputStream
    {
    public:
        explicit StringInputStream(BoundedString& string);
        StringInputStream(BoundedString& string, SoftFail);
    private:
        void Extract(ByteRange range) override;
        void Extract(uint8_t& element) override;
        void Peek(uint8_t& element) override;
        void Forward(std::size_t amount) override;
        bool Empty() const override;
    private:
        BoundedString& string;
    };
}

#endif
