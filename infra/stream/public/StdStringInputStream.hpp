#ifndef INFRA_STD_STRING_INPUT_STREAM_HPP
#define INFRA_STD_STRING_INPUT_STREAM_HPP

#include "infra/stream/public/InputStream.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <cstdint>
#include <string>

namespace infra
{
    class StdStringInputStream
        : private StreamReader
        , public TextInputStream
    {
    public:
        using WithStorage = infra::WithStorage<StdStringInputStream, std::string>;

        explicit StdStringInputStream(std::string& string);
        StdStringInputStream(std::string& string, SoftFail);

    private:
        virtual void Extract(ByteRange range) override;
        virtual void Extract(uint8_t& element) override;
        virtual void Peek(uint8_t& element) override;
        virtual void Forward(std::size_t amount) override;
        virtual bool Empty() const override;

    private:
        std::string& string;
    };
}

#endif
