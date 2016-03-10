#ifndef INFRA_STRING_STREAM_HPP
#define INFRA_STRING_STREAM_HPP

#include "infra/stream/public/InputStreamHelpers.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class StringInputStream
        : public TextInputStreamHelper<char>
    {
    public:
        template<std::size_t Size>
            using WithStorage = infra::WithStorage<StringInputStream, BoundedString::WithStorage<Size>>;

        explicit StringInputStream(BoundedString& string);
        StringInputStream(BoundedString& string, SoftFail);

        void Extract(MemoryRange<char> range) override;
        void Extract(char& element) override;
        void Peek(char& element) override;
        void Forward(std::size_t amount) override;
        bool Empty() const override;

        void ReportFailureCheck(bool hasCheckFailed) override;
        bool HasFailed() const override;
        void ResetFail();

    private:
        BoundedString& string;
    };
}

#endif
