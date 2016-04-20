#ifndef INFRA_COMPARE_HPP
#define INFRA_COMPARE_HPP

// This file contains base classes which provide operators based on provided operators:
//
// class MyClass
//     : public TotallyOrdered<MyClass>
// {
// public:
//     bool operator<(const MyClass& other) const;
//     bool operator==(const MyClass& other) const;
//     // operators !=, >, <=, and >= are now provided by TotallyOrdered
// };

namespace infra
{
    template<class T>
    struct EqualityComparable
    {
        // Requires operator==(const T& x, const T& y) to be already defined
        friend bool operator!=(const T& x, const T& y) { return !(x == y); }
    };

    template<class T>
    struct LessThanComparable
    {
        // Requires operator<(const T& x, const T& y) to be already defined
        friend bool operator>(const T& x, const T& y) { return y < x; }
        friend bool operator<=(const T& x, const T& y) { return !(y < x); }
        friend bool operator>=(const T& x, const T& y) { return !(x < y); }
    };

    template<class T>
    struct TotallyOrdered
        : LessThanComparable<T>
        , EqualityComparable<T>
    {
        // Requires operator==(const T& x, const T& y) to be already defined
        // Requires operator<(const T& x, const T& y) to be already defined
    };

    template<class T, class U>
    struct EqualityComparableHeterogeneous
    {
        // Requires operator==(const T& x, const U& y) to be already defined
        friend bool operator==(const U& y, const T& x) { return x == y; }
        friend bool operator!=(const U& y, const T& x) { return !(x == y); }
        friend bool operator!=(const T& y, const U& x) { return !(y == x); }
    };

    template<class T, class U>
    struct LessThanComparableHeterogeneous
    {
        // Requires operator<(const T& x, const U& y) to be already defined
        // Requires operator>(const T& x, const U& y) to be already defined
        friend bool operator<=(const T& x, const U& y) { return !(x > y); }
        friend bool operator>=(const T& x, const U& y) { return !(x < y); }
        friend bool operator>(const U& x, const T& y)  { return y < x; }
        friend bool operator<(const U& x, const T& y)  { return y > x; }
        friend bool operator<=(const U& x, const T& y) { return !(y < x); }
        friend bool operator>=(const U& x, const T& y) { return !(y > x); }
    };

    template<class T, class U>
    struct TotallyOrderedHeterogeneous
        : LessThanComparableHeterogeneous<T, U>
        , EqualityComparableHeterogeneous<T, U>
    {
        // Requires operator==(const T& x, const U& y) to be already defined
        // Requires operator<(const T& x, const U& y) to be already defined
        // Requires operator>(const T& x, const U& y) to be already defined
    };
}

#endif
