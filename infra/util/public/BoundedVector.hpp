#ifndef INFRA_BOUNDED_VECTOR_HPP
#define INFRA_BOUNDED_VECTOR_HPP

//  BoundedVector is similar to std::vector, except that it can contain a maximum number of elements

#include "infra/util/public/ReallyAssert.hpp"
#include "infra/util/public/StaticStorage.hpp"
#include <algorithm>
#include <iterator>

namespace infra
{

    template<class T, std::size_t Max>
    class BoundedVector
    {
    public:
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef typename std::iterator_traits<iterator>::difference_type difference_type;
        typedef std::size_t size_type;

    public:
        BoundedVector();
        explicit BoundedVector(size_type n, const value_type& value = value_type());
        template<class InputIterator>
            BoundedVector(InputIterator first, InputIterator last);
        BoundedVector(std::initializer_list<T> initializerList);
        BoundedVector(const BoundedVector& other);
        BoundedVector(BoundedVector&& other);
        BoundedVector& operator=(const BoundedVector& other);
        BoundedVector& operator=(BoundedVector&& other);
        ~BoundedVector();

    public:
        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;

        reverse_iterator rbegin();
        const_reverse_iterator rbegin() const;
        reverse_iterator rend();
        const_reverse_iterator rend() const;

        const_iterator cbegin() const;
        const_iterator cend() const;
        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

    public:
        size_type size() const;
        size_type max_size() const;
        void resize(size_type n, const value_type& value = value_type());
        bool empty() const;
        bool full() const;

    public:
        value_type& operator[](size_type position);
        const value_type& operator[](size_type position) const;
        value_type& front();
        const value_type& front() const;
        value_type& back();
        const value_type& back() const;
        value_type* data(); 
        const value_type* data() const;

    public:
        template<class InputIterator>
            void assign(InputIterator first, InputIterator last);
        void assign(size_type n, const value_type& value);

        void push_back(const value_type& value);
        void push_back(value_type&& value);
        void pop_back();

        iterator insert(iterator position, const value_type& value);
        void insert(iterator position, size_type n, const value_type& value);
        template<class InputIterator>
            void insert(iterator position, InputIterator first, InputIterator last);

        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);

        void swap(BoundedVector& other);

        void clear();

        template<class... Args>
            iterator emplace(const_iterator position, Args&&... args);
        template<class... Args>
            void emplace_back(Args&&... args);

    public:
        bool operator==(const BoundedVector& other) const;
        bool operator!=(const BoundedVector& other) const;
        bool operator<(const BoundedVector& other) const;
        bool operator<=(const BoundedVector& other) const;
        bool operator>(const BoundedVector& other) const;
        bool operator>=(const BoundedVector& other) const;

    private:
        void move_up(const_iterator position, size_type n);

    private:
        StaticStorageArray<T, Max> mData;
        size_type mSize;
    };

    template<class T, std::size_t Max>
        void swap(BoundedVector<T, Max>& x, BoundedVector<T, Max>& y);

    //// Implementation ////

    template<class T, std::size_t Max>
    BoundedVector<T, Max>::BoundedVector()
        : mSize(0)
    {}

    template<class T, std::size_t Max>
    BoundedVector<T, Max>::BoundedVector(size_type n, const value_type& value)
        : mSize(0)
    {
        resize(n, value);
    }

    template<class T, std::size_t Max>
    template<class InputIterator>
    BoundedVector<T, Max>::BoundedVector(InputIterator first, InputIterator last)
        : mSize(0)
    {
        assign(first, last);
    }

    template<class T, std::size_t Max>
    BoundedVector<T, Max>::BoundedVector(std::initializer_list<T> initializerList)
        : mSize(0)
    {
        assign(initializerList.begin(), initializerList.end());
    }

    template<class T, std::size_t Max>
    BoundedVector<T, Max>::BoundedVector(const BoundedVector& other)
        : mSize(0)
    {
        assign(other.begin(), other.end());
    }

    template<class T, std::size_t Max>
    BoundedVector<T, Max>::BoundedVector(BoundedVector&& other)
        : mSize(other.mSize)
    {
        for (size_type i = 0; i != size(); ++i)
        {
            mData.Construct(i, std::move(other[i]));
        }
    }

    template<class T, std::size_t Max>
    BoundedVector<T, Max>& BoundedVector<T, Max>::operator=(const BoundedVector& other)
    {
        if (this != &other)
        {
            assign(other.begin(), other.end());
        }

        return *this;
    }

    template<class T, std::size_t Max>
    BoundedVector<T, Max>& BoundedVector<T, Max>::operator=(BoundedVector<T, Max>&& other)
    {
        clear();

        mSize = other.size();

        for (size_type i = 0; i != size(); ++i)
        {
            mData.Construct(i, std::move(other[i]));
        }

        return *this;
    }

    template<class T, std::size_t Max>
    BoundedVector<T, Max>::~BoundedVector()
    {
        clear();
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::iterator BoundedVector<T, Max>::begin()
    {
        return &mData[0];
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::const_iterator BoundedVector<T, Max>::begin() const
    {
        return &mData[0];
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::iterator BoundedVector<T, Max>::end()
    {
        return begin() + size();
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::const_iterator BoundedVector<T, Max>::end() const
    {
        return begin() + size();
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::reverse_iterator BoundedVector<T, Max>::rbegin()
    {
        return reverse_iterator(end());
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::const_reverse_iterator BoundedVector<T, Max>::rbegin() const
    {
        return const_reverse_iterator(end());
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::reverse_iterator BoundedVector<T, Max>::rend()
    {
        return reverse_iterator(begin());
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::const_reverse_iterator BoundedVector<T, Max>::rend() const
    {
        return const_reverse_iterator(begin());
    }


    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::const_iterator BoundedVector<T, Max>::cbegin() const
    {
        return begin();
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::const_iterator BoundedVector<T, Max>::cend() const
    {
        return end();
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::const_reverse_iterator BoundedVector<T, Max>::crbegin() const
    {
        return rbegin();
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::const_reverse_iterator BoundedVector<T, Max>::crend() const
    {
        return rend();
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::size_type BoundedVector<T, Max>::size() const
    {
        return mSize;
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::size_type BoundedVector<T, Max>::max_size() const
    {
        return Max;
    }

    template<class T, std::size_t Max>
    void BoundedVector<T, Max>::resize(size_type n, const value_type& value)
    {
        while (n < size())
        {
            pop_back();
        }

        while (n > size())
        {
            push_back(value);
        }
    }

    template<class T, std::size_t Max>
    bool BoundedVector<T, Max>::empty() const
    {
        return mSize == 0;
    }

    template<class T, std::size_t Max>
    bool BoundedVector<T, Max>::full() const
    {
        return mSize == Max;
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::value_type& BoundedVector<T, Max>::operator[](size_type position)
    {
        really_assert(position >= 0 && position < size());
        return mData[position];
    }

    template<class T, std::size_t Max>
    const typename BoundedVector<T, Max>::value_type& BoundedVector<T, Max>::operator[](size_type position) const
    {
        really_assert(position >= 0 && position < size());
        return mData[position];
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::value_type& BoundedVector<T, Max>::front()
    {
        really_assert(!empty());
        return mData[0];
    }

    template<class T, std::size_t Max>
    const typename BoundedVector<T, Max>::value_type& BoundedVector<T, Max>::front() const
    {
        really_assert(!empty());
        return mData[0];
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::value_type& BoundedVector<T, Max>::back()
    {
        really_assert(!empty());
        return mData[size() - 1];
    }

    template<class T, std::size_t Max>
    const typename BoundedVector<T, Max>::value_type& BoundedVector<T, Max>::back() const
    {
        really_assert(!empty());
        return mData[size() - 1];
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::value_type* BoundedVector<T, Max>::data()
    {
        return begin();
    }

    template<class T, std::size_t Max>
    const typename BoundedVector<T, Max>::value_type* BoundedVector<T, Max>::data() const
    {
        return begin();
    }

    template<class T, std::size_t Max>
    template<class InputIterator>
    void BoundedVector<T, Max>::assign(InputIterator first, InputIterator last)
    {
        clear();

        while (first != last)
        {
            push_back(*first);
            ++first;
        }
    }

    template<class T, std::size_t Max>
    void BoundedVector<T, Max>::assign(size_type n, const value_type& value)
    {
        clear();

        for (size_type i = 0; i != n; ++i)
        {
            push_back(value);
        }
    }

    template<class T, std::size_t Max>
    void BoundedVector<T, Max>::push_back(const value_type& value)
    {
        really_assert(!full());
        mData.Construct(mSize, value);
        ++mSize;
    }

    template<class T, std::size_t Max>
    void BoundedVector<T, Max>::push_back(value_type&& value)
    {
        really_assert(!full());
        mData.Construct(mSize, std::move(value));
        ++mSize;
    }

    template<class T, std::size_t Max>
    void BoundedVector<T, Max>::pop_back()
    {
        really_assert(!empty());
        mData.Destruct(mSize - 1);
        --mSize;
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::iterator BoundedVector<T, Max>::insert(iterator position, const value_type& value)
    {
        insert(position, size_type(1), value);
        return position;
    }

    template<class T, std::size_t Max>
    void BoundedVector<T, Max>::insert(iterator position, size_type n, const value_type& value)
    {
        really_assert(size() + n <= max_size());
        move_up(position, n);

        for (size_type i = 0; i != n; ++i)
        {
            *position = value;
            ++position;
        }
    }

    template<class T, std::size_t Max>
    template<class RandomAccessIterator>
    void BoundedVector<T, Max>::insert(iterator position, RandomAccessIterator first, RandomAccessIterator last)
    {
        size_type n = last - first;
        really_assert(size() + n <= max_size());

        move_up(position, n);

        while (first != last)
        {
            *position = *first;
            ++position;
            ++first;
        }
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::iterator BoundedVector<T, Max>::erase(iterator position)
    {
        erase(position, position + 1);
        return position;
    }

    template<class T, std::size_t Max>
    typename BoundedVector<T, Max>::iterator BoundedVector<T, Max>::erase(iterator first, iterator last)
    {
        iterator result = first;

        while (last != end())
        {
            *first = std::move(*last);
            ++first;
            ++last;
        }

        while (first != last)
        {
            mData.Destruct(first - begin());
            ++first;
            --mSize;
        }

        return result;
    }

    template<class T, std::size_t Max>
    void BoundedVector<T, Max>::swap(BoundedVector& other)
    {
        using std::swap;

        for (size_type i = 0; i < size() && i < other.size(); ++i)
        {
            swap(mData[i], other.mData[i]);
        }

        for (size_type i = size(); i < other.size(); ++i)
        {
            mData.Construct(i, std::move(other.mData[i]));
            other.mData.Destruct(i);
        }

        for (size_type i = other.size(); i < size(); ++i)
        {
            other.mData.Construct(i, std::move(mData[i]));
            mData.Destruct(i);
        }

        std::swap(mSize, other.mSize);
    }

    template<class T, std::size_t Max>
    void BoundedVector<T, Max>::clear()
    {
        while (!empty())
        {
            pop_back();
        }
    }

    template<class T, std::size_t Max>
    template<class... Args>
    typename BoundedVector<T, Max>::iterator BoundedVector<T, Max>::emplace(const_iterator position, Args&&... args)
    {
        really_assert(size() != max_size());
        move_up(position, 1);
        mData.Construct(position - begin(), std::forward<Args>(args)...);
        return begin() + (position - begin());
    }

    template<class T, std::size_t Max>
    template<class... Args>
    void BoundedVector<T, Max>::emplace_back(Args&&... args)
    {
        emplace(end(), std::forward<Args>(args)...);
    }

    template<class T, std::size_t Max>
    bool BoundedVector<T, Max>::operator==(const BoundedVector<T, Max>& other) const
    {
        return size() == other.size()
            && std::equal(begin(), end(), other.begin());
    }

    template<class T, std::size_t Max>
    bool BoundedVector<T, Max>::operator!=(const BoundedVector<T, Max>& other) const
    {
        return !(*this == other);
    }

    template<class T, std::size_t Max>
    bool BoundedVector<T, Max>::operator<(const BoundedVector<T, Max>& other) const
    {
        return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }

    template<class T, std::size_t Max>
    bool BoundedVector<T, Max>::operator<=(const BoundedVector<T, Max>& other) const
    {
        return !(other < *this);
    }

    template<class T, std::size_t Max>
    bool BoundedVector<T, Max>::operator>(const BoundedVector<T, Max>& other) const
    {
        return other < *this;
    }

    template<class T, std::size_t Max>
    bool BoundedVector<T, Max>::operator>=(const BoundedVector<T, Max>& other) const
    {
        return !(*this < other);
    }

    template<class T, std::size_t Max>
    void BoundedVector<T, Max>::move_up(const_iterator position, size_type n)
    {
        iterator copy_position = end();
        while (copy_position != position && copy_position + n != end())
        {
            mData.Construct(copy_position - begin() + n - 1, std::move(*(copy_position - 1)));
            --copy_position;
        }

        while (copy_position != position)
        {
            mData[copy_position - begin() + n - 1] = std::move(*(copy_position - 1));
            --copy_position;
        }

        mSize += n;
    }

    template<class T, std::size_t Max>
    void swap(BoundedVector<T, Max>& x, BoundedVector<T, Max>& y)
    {
        x.swap(y);
    }

}

#endif
