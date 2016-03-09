#ifndef INFRA_STRING_STREAM_HPP
#define INFRA_STRING_STREAM_HPP

#include "infra/stream/public/InputOutputStreamHelpers.hpp"
#include "infra/stream/public/OutputStream.hpp"
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

    class StringOutputStream
        : private OutputStreamWriter
        , public TextOutputStream
    {
    public:
        template<std::size_t Size>
            using WithStorage = infra::WithStorage<StringOutputStream, BoundedString::WithStorage<Size>>;

        explicit StringOutputStream(BoundedString& string);
        StringOutputStream(BoundedString& string, SoftFail);
        ~StringOutputStream();

        bool HasFailed() const;
        void ResetFail();

    private:
        void Insert(ConstByteRange range) override;
        void Insert(uint8_t element) override;
        void Forward(std::size_t amount) override;

    private:
        BoundedString& string;
        bool softFail = false;
        bool failed = false;
        mutable bool checkedFail = true;
    };
}

#endif
