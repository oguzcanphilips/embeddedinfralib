#ifndef INFRA_MEMORY_RANGE_H
#define INFRA_MEMORY_RANGE_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <type_traits>
#include <vector>

namespace infra
{

    template<class T>
    class MemoryRange
    {
    public:
        MemoryRange();
        MemoryRange(T* begin, T* end);
        template<class U>
            MemoryRange(const MemoryRange<U>& other);
        template<class T2, std::size_t N>
            MemoryRange(const std::array<T2, N>& array);
        template<class T2, std::size_t N>
            MemoryRange(std::array<T2, N>& array);
        template<class T2>
            MemoryRange(const std::vector<T2>& vector);
        template<class T2>
            MemoryRange(std::vector<T2>& vector);

        bool empty() const;
        std::size_t size() const;

        // MemoryRange does not own its elements, so accessing an element is a const operation that returns a non-const element
        T* begin() const;
        T* end() const;

        T& operator[](std::size_t index) const;
        bool operator==(const MemoryRange<T>& rhs) const;
        bool operator!=(const MemoryRange<T>& rhs) const;

        T& front() const;
        T& back() const;

        void clear();
        void pop_front(std::size_t num = 1);
        void pop_back(std::size_t num = 1);
        void shrink_from_front_to(std::size_t newSize);
        void shrink_from_back_to(std::size_t newSize);

    private:
        T* mBegin;
        T* mEnd;
    };

    template<class T, std::size_t N>
        MemoryRange<T> MakeRange(std::array<T, N>& data);
    template<class T, std::size_t N>
        MemoryRange<const T> MakeRange(const std::array<T, N>& data);
    template<class T>
        MemoryRange<T> MakeRange(T* b, T* e);

    template<class T, class U>
        MemoryRange<T> ReinterpretCastMemoryRange(MemoryRange<U> memoryRange);
    template<class T>
        MemoryRange<T> ConstCastMemoryRange(MemoryRange<const T> memoryRange);

    template<class T>
        void Copy(MemoryRange<const typename std::remove_const<T>::type> from, MemoryRange<T> to);
    template<class T, class U>
        bool ContentsEqual(MemoryRange<T> x, MemoryRange<U> y);

    template<class T, class U, std::size_t N>
        bool operator==(MemoryRange<T> x, const std::array<U, N>& y);
    template<class T, class U, std::size_t N>
        bool operator==(const std::array<U, N>& x, MemoryRange<T> y);
    template<class T, class U, std::size_t N>
        bool operator!=(MemoryRange<T> x, const std::array<U, N>& y);
    template<class T, class U, std::size_t N>
        bool operator!=(const std::array<U, N>& x, MemoryRange<T> y);

    template<class T, class U>
        bool operator==(MemoryRange<T> x, const std::vector<U>& y);
    template<class T, class U>
        bool operator==(const std::vector<U>& x, MemoryRange<T> y);
    template<class T, class U>
        bool operator!=(MemoryRange<T> x, const std::vector<U>& y);
    template<class T, class U>
        bool operator!=(const std::vector<U>& x, MemoryRange<T> y);

    ////    Implementation    ////

    template<class T>
    MemoryRange<T>::MemoryRange()
        : mBegin(nullptr)
        , mEnd(nullptr)
    {}

    template<class T>
    MemoryRange<T>::MemoryRange(T* begin, T* end)
        : mBegin(begin)
        , mEnd(end)
    {}

    template<class T>
    template<class U>
    MemoryRange<T>::MemoryRange(const MemoryRange<U>& other)
        : mBegin(other.begin())
        , mEnd(other.end())
    {}

    template<class T>
    template<class T2, std::size_t N>
    MemoryRange<T>::MemoryRange(const std::array<T2, N>& array)
        : mBegin(array.data())
        , mEnd(array.data() + array.size())
    {}

    template<class T>
    template<class T2, std::size_t N>
    MemoryRange<T>::MemoryRange(std::array<T2, N>& array)
        : mBegin(array.data())
        , mEnd(array.data() + array.size())
    {}

    template<class T>
    template<class T2>
    MemoryRange<T>::MemoryRange(const std::vector<T2>& vector)
        : mBegin(vector.data())
        , mEnd(vector.data() + vector.size())
    {}

    template<class T>
    template<class T2>
    MemoryRange<T>::MemoryRange(std::vector<T2>& vector)
        : mBegin(vector.data())
        , mEnd(vector.data() + vector.size())
    {}

    template<class T>
    bool MemoryRange<T>::empty() const
    {
        return mBegin == mEnd;
    }

    template<class T>
    std::size_t MemoryRange<T>::size() const
    {
        return mEnd - mBegin;
    }

    template<class T>
    T* MemoryRange<T>::begin() const
    {
        return mBegin;
    }

    template<class T>
    T* MemoryRange<T>::end() const
    {
        return mEnd;
    }

    template<class T>
    T& MemoryRange<T>::operator[](std::size_t index) const
    {
        assert(index < size());
        return *(begin() + index);
    }

    template<class T>
    bool MemoryRange<T>::operator==(const MemoryRange<T>& rhs) const
    {
        return mBegin == rhs.mBegin && mEnd == rhs.mEnd;
    }

    template<class T>
    bool MemoryRange<T>::operator!=(const MemoryRange<T>& rhs) const
    {
        return !(*this == rhs);
    }

    template<class T>
    T& MemoryRange<T>::front() const
    {
        return *mBegin;
    }

    template<class T>
    T& MemoryRange<T>::back() const
    {
        return *(mEnd - 1);
    }

    template<class T>
    void MemoryRange<T>::clear()
    {
        mBegin = nullptr;
        mEnd = nullptr;
    }

    template<class T>
    void MemoryRange<T>::pop_front(std::size_t num)
    {
        assert(num <= size());
        mBegin += num;
    }

    template<class T>
    void MemoryRange<T>::pop_back(std::size_t num)
    {
        assert(num <= size());
        mEnd -= num;
    }

    template<class T>
    void MemoryRange<T>::shrink_from_front_to(std::size_t newSize)
    {
        if (newSize < size())
            mBegin = mEnd - newSize;
    }

    template<class T>
    void MemoryRange<T>::shrink_from_back_to(std::size_t newSize)
    {
        if (newSize < size())
            mEnd = mBegin + newSize;
    }

    template<class T, std::size_t N>
    MemoryRange<T> MakeRange(std::array<T, N>& data)
    {
        return MemoryRange<T>(data.data(), data.data() + data.size());
    }

    template<class T, std::size_t N>
    MemoryRange<const T> MakeRange(const std::array<T, N>& data)
    {
        return MemoryRange<const T>(data.data(), data.data() + data.size());
    }

    template<class T>
    MemoryRange<T> MakeRange(T* b, T* e)
    {
        return MemoryRange<T>(b, e);
    }

    template<class T, class U>
    MemoryRange<T> ReinterpretCastMemoryRange(MemoryRange<U> memoryRange)
    {
        return MemoryRange<T>(reinterpret_cast<T*>(memoryRange.begin()), reinterpret_cast<T*>(memoryRange.end()));
    }

    template<class T>
    MemoryRange<T> ConstCastMemoryRange(MemoryRange<const T> memoryRange)
    {
        return MemoryRange<T>(const_cast<T*>(memoryRange.begin()), const_cast<T*>(memoryRange.end()));
    }

    template<class T>
    void Copy(MemoryRange<const typename std::remove_const<T>::type> from, MemoryRange<T> to)
    {
        assert(from.size() == to.size());
        std::copy(from.begin(), from.begin() + from.size(), to.begin());
    }

    template<class T, class U>
    bool ContentsEqual(MemoryRange<T> x, MemoryRange<U> y)
    {
        return x.size() == y.size()
            && std::equal(x.begin(), x.end(), y.begin());
    }

    template<class T, class U, std::size_t N>
    bool operator==(MemoryRange<T> x, const std::array<U, N>& y)
    {
        return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
    }

    template<class T, class U, std::size_t N>
    bool operator==(const std::array<U, N>& x, MemoryRange<T> y)
    {
        return y == x;
    }

    template<class T, class U, std::size_t N>
    bool operator!=(MemoryRange<T> x, const std::array<U, N>& y)
    {
        return !(x == y);
    }

    template<class T, class U, std::size_t N>
    bool operator!=(const std::array<U, N>& x, MemoryRange<T> y)
    {
        return !(x == y);
    }

    template<class T, class U>
    bool operator==(MemoryRange<T> x, const std::vector<U>& y)
    {
        return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
    }

    template<class T, class U>
    bool operator==(const std::vector<U>& x, MemoryRange<T> y)
    {
        return y == x;
    }

    template<class T, class U>
    bool operator!=(MemoryRange<T> x, const std::vector<U>& y)
    {
        return !(x == y);
    }

    template<class T, class U>
    bool operator!=(const std::vector<U>& x, MemoryRange<T> y)
    {
        return !(x == y);
    }

}

#endif
