#ifndef INFRA_STRING_OUTPUT_STREAM_HPP
#define INFRA_STRING_OUTPUT_STREAM_HPP

#include "infra/stream/public/StreamManipulators.hpp"
#include "infra/stream/public/OutputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class StringOutputStream
        : private StreamWriter
        , public TextOutputStream
    {
    public:
        template<std::size_t Size>
            using WithStorage = infra::WithStorage<StringOutputStream, BoundedString::WithStorage<Size>>;

        explicit StringOutputStream(BoundedString& string);
        StringOutputStream(BoundedString& string, SoftFail);
        ~StringOutputStream() = default;
    private:
        virtual void Insert(ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;
        virtual void Forward(std::size_t amount) override;

    private:
        BoundedString& string;
    };
}

#endif
