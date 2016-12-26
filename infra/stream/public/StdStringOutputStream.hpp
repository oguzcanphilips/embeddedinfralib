#ifndef INFRA_STD_STRING_OUTPUT_STREAM_HPP
#define INFRA_STD_STRING_OUTPUT_STREAM_HPP

#include "infra/stream/public/OutputStream.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <cstdint>
#include <string>

namespace infra
{
    class StdStringOutputStream                                                         //TICS !OOP#013
        : private StreamWriter
        , public TextOutputStream
    {
    public:
        using WithStorage = infra::WithStorage<StdStringOutputStream, std::string>;

        explicit StdStringOutputStream(std::string& string);
        StdStringOutputStream(std::string& string, SoftFail);
        StdStringOutputStream(std::string& string, NoFail);
        ~StdStringOutputStream();

    private:
        virtual void Insert(ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;
        virtual void Forward(std::size_t amount) override;

    private:
        std::string& string;
    };
}

#endif
