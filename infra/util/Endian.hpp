#ifndef INFRA_ENDIAN_HPP
#define INFRA_ENDIAN_HPP

#include "infra/util/ByteRange.hpp"
#include <type_traits>

namespace infra
{
    template<class T>
    T SwapEndian(T value)
    {
        static_assert(std::is_standard_layout<T>::value, "T should be standard layout");

        T result;
        auto valueRange = infra::MakeByteRange(value);
        std::copy(valueRange.begin(), valueRange.end(), std::make_reverse_iterator(infra::MakeByteRange(result).end()));
        return result;
    }

    template<class T>
    class BigEndian
    {
    public:
        BigEndian() = default;

        BigEndian(T value)
            : value(SwapEndian(value))
        {}

        operator T() const
        {
            return SwapEndian(value);
        }

        bool operator==(BigEndian other) const
        {
            return value == other.value;
        }

        bool operator!=(BigEndian other) const
        {
            return !(*this == other);
        }

        bool operator==(T other) const
        {
            return value == SwapEndian(other);
        }

        bool operator!=(T other) const
        {
            return !(*this == other);
        }

        friend bool operator==(T x, BigEndian y)
        {
            return y == x;
        }

        friend bool operator!=(T x, BigEndian y)
        {
            return y != x;
        }

    private:
        T value = 0;
    };
}

#endif
