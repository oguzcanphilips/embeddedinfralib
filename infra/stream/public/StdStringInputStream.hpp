#ifndef INFRA_STD_STRING_INPUT_STREAM_HPP
#define INFRA_STD_STRING_INPUT_STREAM_HPP

#include "infra/stream/public/InputStreamHelpers.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <cstdint>
#include <string>

namespace infra
{
    class StdStringInputStream
        : public TextInputStreamHelper<char>
    {
    public:
        using WithStorage = infra::WithStorage<StdStringInputStream, std::string>;

        explicit StdStringInputStream(std::string& string);
        StdStringInputStream(std::string& string, SoftFail);

        void Extract(MemoryRange<char> range) override;
        void Extract(char& element) override;
        void Peek(char& element) override;
        void Forward(std::size_t amount) override;
        bool Empty() const override;
        void ReportFailureCheck(bool hasCheckFailed) override;
        bool HasFailed() const override;
        void ResetFail();

    private:
        std::string& string;
    };
}

#endif
