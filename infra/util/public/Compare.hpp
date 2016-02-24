#ifndef INFRA_COMPARE_HPP
#define INFRA_COMPARE_HPP

// This file contains classes which make comparison of objects with many datamembers easier. It is used like this:
// 
// bool operator==(const MyClass& x, const MyClass& y)
// {
//     return infra::Equals()
//         (x.a, y.a)
//         (x.b, y.b);
// }
// 
// bool operator<(const MyClass& x, const MyClass& y)
// {
//     return infra::LessThan()
//         (x.a, y.a)
//         (x.b, y.b);
// }
//
// Furthermore, base classes are added which provide operators based on provided operators:
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

    class Equals
    {
    public:
        Equals();

        template<class T>
            Equals& operator()(const T& x, const T& y);

        operator bool() const;
        bool operator!() const;

    private:
        bool equal;
    };

    class LessThan
    {
    public:
        LessThan();

        template<class T>
            LessThan& operator()(const T& x, const T& y);

        operator bool() const;
        bool operator!() const;

    private:
        bool lessThan;
        bool equal;
    };

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

    ////    Implementation    ////

    inline Equals::Equals()
        : equal(true)
    {}

    template<class T>
    inline Equals& Equals::operator()(const T& x, const T& y)
    {
        equal &= x == y;
        return *this;
    }

    inline Equals::operator bool() const
    {
        return equal;
    }

    inline bool Equals::operator!() const
    {
        return !equal;
    }

    inline LessThan::LessThan()
        : lessThan(false)
        , equal(true)
    {}

    template<class T>
    inline LessThan& LessThan::operator()(const T& x, const T& y)
    {
        lessThan = lessThan || (equal && (x < y));
        equal = equal && (x == y);

        return *this;
    }

    inline LessThan::operator bool() const
    {
        return lessThan;
    }

    inline bool LessThan::operator!() const
    {
        return !lessThan;
    }

}

#endif
