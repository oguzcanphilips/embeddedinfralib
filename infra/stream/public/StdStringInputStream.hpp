#ifndef INFRA_STD_STRING_INPUT_STREAM_HPP
#define INFRA_STD_STRING_INPUT_STREAM_HPP

#include "infra/stream/public/InputStream.hpp"
#include <cstdint>
#include <string>

namespace infra
{
    class StdStringInputStream
        : private StreamReader
        , public TextInputStream
    {
    public:
        explicit StdStringInputStream(const std::string& string);
        StdStringInputStream(const std::string& string, SoftFail);
    private:
        void Extract(ByteRange range) override;
        void Extract(uint8_t& element) override;
        void Peek(uint8_t& element) override;
        void Forward(std::size_t amount) override;
        bool Empty() const override;
    private:
        std::string string;
    };
}

#endif
