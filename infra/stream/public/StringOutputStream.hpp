#ifndef INFRA_STRING_OUTPUT_STREAM_HPP
#define INFRA_STRING_OUTPUT_STREAM_HPP

#include "infra/stream/public/StreamManipulators.hpp"
#include "infra/stream/public/OutputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

namespace infra
{
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
