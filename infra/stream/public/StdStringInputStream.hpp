#ifndef INFRA_STD_STRING_INPUT_STREAM_HPP
#define INFRA_STD_STRING_INPUT_STREAM_HPP

#include "infra/stream/public/InputStream.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <cstdint>
#include <string>

namespace infra
{
    class StdStringInputStream                                                          //TICS !OOP#013
        : private StreamReader
        , public TextInputStream
    {
    public:
        using WithStorage = infra::WithStorage<StdStringInputStream, std::string>;

        explicit StdStringInputStream(const std::string& string);
        StdStringInputStream(const std::string& string, SoftFail);

    private:
        virtual void Extract(ByteRange range) override;
        virtual uint8_t ExtractOne() override;
        virtual uint8_t Peek() override;
        virtual bool Empty() const override;

    private:
        uint32_t offset = 0;
        const std::string& string;
    };
}

#endif
