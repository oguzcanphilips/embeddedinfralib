#ifndef INFRA_FIXED_POINT_HPP
#define INFRA_FIXED_POINT_HPP

#include "infra/util/public/Compare.hpp"

namespace infra
{

    struct Scaled {};
    const Scaled scaled;
    struct Unscaled {};
    const Unscaled unscaled;

    template<class T, T Factor>
    struct FixedPoint
        : TotallyOrdered<FixedPoint<T, Factor>>
        , TotallyOrderedHeterogeneous<FixedPoint<T, Factor>, T>
    {
        FixedPoint()
            : value(0)
        {}

        FixedPoint(Scaled, T v)
            : value(v * Factor)
        {}

        FixedPoint(Unscaled, T v)
            : value(v)
        {}

        T AsUnscaled() const { return value; }
        T IntegerPart() const { return value / Factor; }
        T FractionalPart() const { return value % Factor; }

        FixedPoint& operator+=(const FixedPoint& x) { value += x.value; return *this; }
        FixedPoint& operator-=(const FixedPoint& x) { value -= x.value; return *this; }
        FixedPoint& operator*=(const FixedPoint& x) { value *= x.value; value /= Factor; return *this; }
        FixedPoint& operator/=(const FixedPoint& x) { value *= Factor; value /= x.value; return *this; }
        FixedPoint& operator*=(T x) { value *= x; return *this; }
        FixedPoint& operator/=(T x) { value /= x; return *this; }
        FixedPoint operator-() { FixedPoint result; result.value = -value; return result; }

        friend FixedPoint operator+(FixedPoint x, const FixedPoint& y) { return x += y; }
        friend FixedPoint operator-(FixedPoint x, const FixedPoint& y) { return x -= y; }
        friend FixedPoint operator*(FixedPoint x, const FixedPoint& y) { return x *= y; }
        friend FixedPoint operator/(FixedPoint x, const FixedPoint& y) { return x /= y; }
        friend FixedPoint operator*(FixedPoint x, T y) { return x *= y; }
        friend FixedPoint operator*(T x, FixedPoint y) { return y *= x; }
        friend FixedPoint operator/(FixedPoint x, T y) { return x /= y; }

        friend bool operator==(const FixedPoint& x, const FixedPoint& y) { return x.value == y.value; }
        friend bool operator<(const FixedPoint& x, const FixedPoint& y) { return x.value < y.value; }
        friend bool operator==(const FixedPoint& x, const T& y) { return x.value == y * Factor; }
        friend bool operator<(const FixedPoint& x, const T& y) { return x.value < y * Factor; }
        friend bool operator>(const FixedPoint& x, const T& y) { return x.value > y * Factor; }

    private:
        T value;
    };

}

#endif
