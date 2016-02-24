#ifndef INFRA_BOUNDED_DEQUE_HPP
#define INFRA_BOUNDED_DEQUE_HPP

//  BoundedDeque is similar to std::deque, except that it can contain a maximum number of elements.

#include "infra/util/public/ReallyAssert.hpp"
#include "infra/util/public/StaticStorage.hpp"
#include <algorithm>
#include <iterator>

namespace infra
{
    namespace detail
    {
        template<class DequeType, class T>
            class BoundedDequeIterator;
    }

    template<class T, std::size_t Max>
    class BoundedDeque
    {
    public:
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef detail::BoundedDequeIterator<BoundedDeque<T, Max>, T> iterator;
        typedef detail::BoundedDequeIterator<const BoundedDeque<T, Max>, const T> const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef typename std::iterator_traits<iterator>::difference_type difference_type;
        typedef std::size_t size_type;

    public:
        BoundedDeque();
        explicit BoundedDeque(size_type n, const value_type& value = value_type());
        template<class InputIterator>
            BoundedDeque(InputIterator first, InputIterator last);
        BoundedDeque(const BoundedDeque& other);
        BoundedDeque(BoundedDeque&& other);
        BoundedDeque(std::initializer_list<T> initializerList);
        BoundedDeque& operator=(const BoundedDeque& other);
        BoundedDeque& operator=(BoundedDeque&& other);
        BoundedDeque& operator= (std::initializer_list<T> initializerList);
        ~BoundedDeque();

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

    public:
        template<class InputIterator>
            void assign(InputIterator first, const InputIterator& last);
        void assign(size_type n, const value_type& value);
        void assign(std::initializer_list<T> initializerList);

        void push_front(const value_type& value);
        void push_front(value_type&& value);
        void pop_front();
        void push_back(const value_type& value);
        void push_back(value_type&& value);
        void pop_back();

        iterator insert(const const_iterator& position, const value_type& value);
        iterator insert(const const_iterator& position, size_type n, const value_type& value);
        template<class InputIterator>
            iterator insert(const const_iterator& position, InputIterator first, const InputIterator& last);
        iterator insert(const const_iterator& position, T&& val);
        iterator insert(const const_iterator& position, std::initializer_list<T> initializerList);

        iterator erase(const const_iterator& position);
        iterator erase(const const_iterator& first, const const_iterator& last);

        void swap(BoundedDeque& other);

        void clear();

        template<class... Args>
            iterator emplace(const const_iterator& position, Args&&... args);
        template<class... Args>
            void emplace_front(Args&&... args);
        template<class... Args>
            void emplace_back(Args&&... args);

    public:
        bool operator==(const BoundedDeque& other) const;
        bool operator!=(const BoundedDeque& other) const;
        bool operator<(const BoundedDeque& other) const;
        bool operator<=(const BoundedDeque& other) const;
        bool operator>(const BoundedDeque& other) const;
        bool operator>=(const BoundedDeque& other) const;

    private:
        size_type index(size_type i) const;
        void move_up(size_type position, size_type n);

    private:
        StaticStorageArray<T, Max> mData;
        size_type mSize;
        size_type mStart;
    };

    template<class T, std::size_t Max>
        void swap(BoundedDeque<T, Max>& x, BoundedDeque<T, Max>& y);

    namespace detail
    {
        template<class DequeType, class T>
        class BoundedDequeIterator
            : public std::iterator<std::random_access_iterator_tag, T>
        {
        public:
            BoundedDequeIterator();

            BoundedDequeIterator(DequeType* deque, std::size_t offset);

            template<class DequeType2, class T2>
                BoundedDequeIterator(const BoundedDequeIterator<DequeType2, T2>& other);

            template<class DequeType2, class T2>
                BoundedDequeIterator& operator=(const BoundedDequeIterator<DequeType2, T2>& other);

            T& operator*() const;
            T* operator->() const;
            T& operator[](std::ptrdiff_t offset) const;
            BoundedDequeIterator& operator++();
            BoundedDequeIterator operator++(int);
            BoundedDequeIterator& operator--();
            BoundedDequeIterator operator--(int);

            BoundedDequeIterator& operator+=(std::ptrdiff_t offset);
            BoundedDequeIterator operator+(std::ptrdiff_t offset) const;
            BoundedDequeIterator& operator-=(std::ptrdiff_t offset);
            BoundedDequeIterator operator-(std::ptrdiff_t offset) const;
            std::ptrdiff_t operator-(BoundedDequeIterator other) const;

            template<class DequeType2, class T2>
                bool operator==(const BoundedDequeIterator<DequeType2, T2>& other) const;
            template<class DequeType2, class T2>
                bool operator!=(const BoundedDequeIterator<DequeType2, T2>& other) const;
            template<class DequeType2, class T2>
                bool operator<(const BoundedDequeIterator<DequeType2, T2>& other) const;
            template<class DequeType2, class T2>
                bool operator>(const BoundedDequeIterator<DequeType2, T2>& other) const;
            template<class DequeType2, class T2>
                bool operator<=(const BoundedDequeIterator<DequeType2, T2>& other) const;
            template<class DequeType2, class T2>
                bool operator>=(const BoundedDequeIterator<DequeType2, T2>& other) const;

        private:
            template<class, class>
                friend class BoundedDequeIterator;

            std::size_t mIndex;
            DequeType* mDeque;
        };

    }

    //// Implementation ////

    template<class T, std::size_t Max>
    BoundedDeque<T, Max>::BoundedDeque()
        : mSize(0)
        , mStart(0)
    {}

    template<class T, std::size_t Max>
    BoundedDeque<T, Max>::BoundedDeque(size_type n, const value_type& value)
        : mSize(0)
        , mStart(0)
    {
        resize(n, value);
    }

    template<class T, std::size_t Max>
    template<class InputIterator>
    BoundedDeque<T, Max>::BoundedDeque(InputIterator first, InputIterator last)
        : mSize(0)
        , mStart(0)
    {
        assign(first, last);
    }

    template<class T, std::size_t Max>
    BoundedDeque<T, Max>::BoundedDeque(const BoundedDeque& other)
        : mSize(0)
        , mStart(0)
    {
        assign(other.begin(), other.end());
    }

    template<class T, std::size_t Max>
    BoundedDeque<T, Max>::BoundedDeque(BoundedDeque&& other)
        : mSize(other.mSize)
        , mStart(0)
    {
        for (size_type i = 0; i != size(); ++i)
            mData.Construct(i, std::move(other[i]));
    }

    template<class T, std::size_t Max>
    BoundedDeque<T, Max>::BoundedDeque(std::initializer_list<T> initializerList)
        : mSize(0)
        , mStart(0)
    {
        assign(initializerList.begin(), initializerList.end());
    }

    template<class T, std::size_t Max>
    BoundedDeque<T, Max>& BoundedDeque<T, Max>::operator=(const BoundedDeque& other)
    {
        if (this != &other)
            assign(other.begin(), other.end());

        return *this;
    }

    template<class T, std::size_t Max>
    BoundedDeque<T, Max>& BoundedDeque<T, Max>::operator=(BoundedDeque<T, Max>&& other)
    {
        clear();

        mSize = other.mSize;

        for (size_type i = 0; i != size(); ++i)
            mData.Construct(i, std::move(other[i]));

        return *this;
    }

    template<class T, std::size_t Max>
    BoundedDeque<T, Max>& BoundedDeque<T, Max>::operator=(std::initializer_list<T> initializerList)
    {
        assign(initializerList.begin(), initializerList.end());

        return *this;
    }

    template<class T, std::size_t Max>
    BoundedDeque<T, Max>::~BoundedDeque()
    {
        clear();
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::begin()
    {
        return iterator(this, 0);
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::const_iterator BoundedDeque<T, Max>::begin() const
    {
        return const_iterator(this, 0);
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::end()
    {
        return iterator(this, mSize);
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::const_iterator BoundedDeque<T, Max>::end() const
    {
        return const_iterator(this, mSize);
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::reverse_iterator BoundedDeque<T, Max>::rbegin()
    {
        return reverse_iterator(end());
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::const_reverse_iterator BoundedDeque<T, Max>::rbegin() const
    {
        return const_reverse_iterator(end());
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::reverse_iterator BoundedDeque<T, Max>::rend()
    {
        return reverse_iterator(begin());
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::const_reverse_iterator BoundedDeque<T, Max>::rend() const
    {
        return const_reverse_iterator(begin());
    }
    
    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::const_iterator BoundedDeque<T, Max>::cbegin() const
    {
        return begin();
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::const_iterator BoundedDeque<T, Max>::cend() const
    {
        return end();
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::const_reverse_iterator BoundedDeque<T, Max>::crbegin() const
    {
        return rbegin();
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::const_reverse_iterator BoundedDeque<T, Max>::crend() const
    {
        return rend();
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::size_type BoundedDeque<T, Max>::size() const
    {
        return mSize;
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::size_type BoundedDeque<T, Max>::max_size() const
    {
        return Max;
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::resize(size_type n, const value_type& value)
    {
        while (n < size())
            pop_back();

        while (n > size())
            push_back(value);
    }

    template<class T, std::size_t Max>
    bool BoundedDeque<T, Max>::empty() const
    {
        return mSize == 0;
    }

    template<class T, std::size_t Max>
    bool BoundedDeque<T, Max>::full() const
    {
        return mSize == Max;
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::value_type& BoundedDeque<T, Max>::operator[](size_type position)
    {
        really_assert(position >= 0 && position < size());
        return mData[index(position)];
    }

    template<class T, std::size_t Max>
    const typename BoundedDeque<T, Max>::value_type& BoundedDeque<T, Max>::operator[](size_type position) const
    {
        really_assert(position >= 0 && position < size());
        return mData[index(position)];
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::value_type& BoundedDeque<T, Max>::front()
    {
        really_assert(!empty());
        return mData[mStart];
    }

    template<class T, std::size_t Max>
    const typename BoundedDeque<T, Max>::value_type& BoundedDeque<T, Max>::front() const
    {
        really_assert(!empty());
        return mData[mStart];
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::value_type& BoundedDeque<T, Max>::back()
    {
        really_assert(!empty());
        return mData[index(mSize - 1)];
    }

    template<class T, std::size_t Max>
    const typename BoundedDeque<T, Max>::value_type& BoundedDeque<T, Max>::back() const
    {
        really_assert(!empty());
        return mData[index(mSize - 1)];
    }

    template<class T, std::size_t Max>
    template<class InputIterator>
    void BoundedDeque<T, Max>::assign(InputIterator first, const InputIterator& last)
    {
        clear();

        for (; first != last; ++first)
            push_back(*first);
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::assign(size_type n, const value_type& value)
    {
        clear();

        for (size_type i = 0; i != n; ++i)
            push_back(value);
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::assign(std::initializer_list<T> initializerList)
    {
        assign(initializerList.begin(), initializerList.end());
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::push_front(const value_type& value)
    {
        really_assert(!full());
        mStart = index(Max - 1);
        mData.Construct(mStart, value);
        ++mSize;
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::push_front(value_type&& value)
    {
        really_assert(!full());
        mStart = index(Max - 1);
        mData.Construct(mStart, std::move(value));
        ++mSize;
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::pop_front()
    {
        really_assert(!empty());
        mData.Destruct(mStart);
        mStart = index(1);
        --mSize;
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::push_back(const value_type& value)
    {
        really_assert(!full());
        mData.Construct(index(mSize), value);
        ++mSize;
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::push_back(value_type&& value)
    {
        really_assert(!full());
        mData.Construct(index(mSize), std::move(value));
        ++mSize;
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::pop_back()
    {
        really_assert(!empty());
        mData.Destruct(index(mSize - 1));
        --mSize;
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::insert(const const_iterator& position, const value_type& value)
    {
        return insert(position, size_type(1), value);
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::insert(const const_iterator& position, size_type n, const value_type& value)
    {
        really_assert(size() + n <= max_size());
        size_type element_index = position - begin();
        move_up(element_index, n);

        iterator pos(this, position - cbegin());
        for (size_type i = 0, e = element_index; i != n; ++i, ++e)
            mData[index(e)] = value;

        return iterator(this, element_index);
    }

    template<class T, std::size_t Max>
    template<class RandomAccessIterator>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::insert(const const_iterator& position, RandomAccessIterator first, const RandomAccessIterator& last)
    {
        size_type n = last - first;
        really_assert(size() + n <= max_size());
        size_type element_index = position - begin();
        move_up(element_index, n);

        for (; first != last; ++element_index, ++first)
            mData[index(element_index)] = *first;

        return iterator(this, position - begin());
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::insert(const const_iterator& position, T&& val)
    {
        return emplace(position, std::move(val));
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::insert(const const_iterator& position, std::initializer_list<T> initializerList)
    {
        return insert(position, initializerList.begin(), initializerList.end());
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::erase(const const_iterator& position)
    {
        return erase(position, position + 1);
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::erase(const const_iterator& first, const const_iterator& last)
    {
        size_type first_index = first - begin();
        size_type last_index = last - begin();

        for (; last_index != mSize; ++first_index, ++last_index)
            mData[index(first_index)] = std::move(mData[index(last_index)]);

        for (; first_index != last_index; ++first_index, --mSize)
            mData.Destruct(index(first_index));

        return iterator(this, first - begin());
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::swap(BoundedDeque& other)
    {
        using std::swap;

        for (size_type i = 0; i < size() && i < other.size(); ++i)
            swap(mData[index(i)], other.mData[other.index(i)]);

        for (size_type i = size(); i < other.size(); ++i)
        {
            mData.Construct(index(i), std::move(other.mData[other.index(i)]));
            other.mData.Destruct(other.index(i));
        }

        for (size_type i = other.size(); i < size(); ++i)
        {
            other.mData.Construct(other.index(i), std::move(mData[index(i)]));
            mData.Destruct(index(i));
        }

        std::swap(mSize, other.mSize);
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::clear()
    {
        if (std::is_trivially_destructible<T>::value)
            mSize = 0;
        else
            while (!empty())
                pop_back();
    }

    template<class T, std::size_t Max>
    template<class... Args>
    typename BoundedDeque<T, Max>::iterator BoundedDeque<T, Max>::emplace(const const_iterator& position, Args&&... args)
    {
        really_assert(size() != max_size());
        size_type element_index = position - begin();
        move_up(element_index, 1);
        mData.Construct(index(element_index), std::forward<Args>(args)...);
        
        return iterator(this, element_index);
    }

    template<class T, std::size_t Max>
    template<class... Args>
    void BoundedDeque<T, Max>::emplace_front(Args&&... args)
    {
        really_assert(size() != max_size());
        if (mStart != 0)
            --mStart;
        else
            mStart = Max - 1;
        mData.Construct(index(0), std::forward<Args>(args)...);
        ++mSize;
    }

    template<class T, std::size_t Max>
    template<class... Args>
    void BoundedDeque<T, Max>::emplace_back(Args&&... args)
    {
        emplace(end(), std::forward<Args>(args)...);
    }

    template<class T, std::size_t Max>
    bool BoundedDeque<T, Max>::operator==(const BoundedDeque<T, Max>& other) const
    {
        return size() == other.size() && std::equal(begin(), end(), other.begin());
    }

    template<class T, std::size_t Max>
    bool BoundedDeque<T, Max>::operator!=(const BoundedDeque<T, Max>& other) const
    {
        return !(*this == other);
    }

    template<class T, std::size_t Max>
    bool BoundedDeque<T, Max>::operator<(const BoundedDeque<T, Max>& other) const
    {
        return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }

    template<class T, std::size_t Max>
    bool BoundedDeque<T, Max>::operator<=(const BoundedDeque<T, Max>& other) const
    {
        return !(other < *this);
    }

    template<class T, std::size_t Max>
    bool BoundedDeque<T, Max>::operator>(const BoundedDeque<T, Max>& other) const
    {
        return other < *this;
    }

    template<class T, std::size_t Max>
    bool BoundedDeque<T, Max>::operator>=(const BoundedDeque<T, Max>& other) const
    {
        return !(*this < other);
    }

    template<class T, std::size_t Max>
    typename BoundedDeque<T, Max>::size_type BoundedDeque<T, Max>::index(size_type elementIndex) const
    {
        size_type index = mStart + elementIndex;
        if (index < Max)
            return index;
        else
            return index - Max;
    }

    template<class T, std::size_t Max>
    void BoundedDeque<T, Max>::move_up(size_type position, size_type n)
    {
        size_type number_of_copies = mSize - position;
        size_type copy_position = mSize - 1 + n;

        for (; number_of_copies != 0 && copy_position >= mSize; --number_of_copies, --copy_position)
            mData.Construct(index(copy_position), std::move(mData[index(copy_position - n)]));

        for (; number_of_copies != 0; --number_of_copies, --copy_position)
            mData[index(copy_position)] = std::move(mData[index(copy_position - n)]);

        mSize += n;
    }

    template<class T, std::size_t Max>
    void swap(BoundedDeque<T, Max>& x, BoundedDeque<T, Max>& y)
    {
        x.swap(y);
    }

    namespace detail
    {
        template<class DequeType, class T>
        BoundedDequeIterator<DequeType, T>::BoundedDequeIterator(DequeType* deque, std::size_t offset)
            : mIndex(offset)
            , mDeque(deque)
        {}

        template<class DequeType, class T>
        template<class DequeType2, class T2>
        BoundedDequeIterator<DequeType, T>::BoundedDequeIterator(const BoundedDequeIterator<DequeType2, T2>& other)
            : mIndex(other.mIndex)
            , mDeque(other.mDeque)
        {}

        template<class DequeType, class T>
        template<class DequeType2, class T2>
        BoundedDequeIterator<DequeType, T>& BoundedDequeIterator<DequeType, T>::operator=(const BoundedDequeIterator<DequeType2, T2>& other)
        {
            mDeque = other.mDeque;
            mIndex = other.mIndex;

            return *this;
        }

        template<class DequeType, class T>
        T& BoundedDequeIterator<DequeType, T>::operator*() const
        {
            return (*mDeque)[mIndex];
        }

        template<class DequeType, class T>
        T* BoundedDequeIterator<DequeType, T>::operator->() const
        {
            return &(*mDeque)[mIndex];
        }

        template<class DequeType, class T>
        T& BoundedDequeIterator<DequeType, T>::operator[](std::ptrdiff_t offset) const
        {
            return (*mDeque)[mIndex + offset];
        }

        template<class DequeType, class T>
        BoundedDequeIterator<DequeType, T>& BoundedDequeIterator<DequeType, T>::operator++()
        {
            ++mIndex;
            return *this;
        }

        template<class DequeType, class T>
        BoundedDequeIterator<DequeType, T> BoundedDequeIterator<DequeType, T>::operator++(int)
        {
            BoundedDequeIterator copy(*this);
            ++mIndex;
            return copy;
        }

        template<class DequeType, class T>
        BoundedDequeIterator<DequeType, T>& BoundedDequeIterator<DequeType, T>::operator--()
        {
            --mIndex;
            return *this;
        }

        template<class DequeType, class T>
        BoundedDequeIterator<DequeType, T> BoundedDequeIterator<DequeType, T>::operator--(int)
        {
            BoundedDequeIterator copy(*this);
            --mIndex;
            return copy;
        }

        template<class DequeType, class T>
        BoundedDequeIterator<DequeType, T>& BoundedDequeIterator<DequeType, T>::operator+=(std::ptrdiff_t offset)
        {
            mIndex += offset;
            return *this;
        }

        template<class DequeType, class T>
        BoundedDequeIterator<DequeType, T> BoundedDequeIterator<DequeType, T>::operator+(std::ptrdiff_t offset) const
        {
            BoundedDequeIterator copy(*this);
            copy += offset;
            return copy;
        }

        template<class DequeType, class T>
        BoundedDequeIterator<DequeType, T>& BoundedDequeIterator<DequeType, T>::operator-=(std::ptrdiff_t offset)
        {
            mIndex -= offset;
            return *this;
        }

        template<class DequeType, class T>
        BoundedDequeIterator<DequeType, T> BoundedDequeIterator<DequeType, T>::operator-(std::ptrdiff_t offset) const
        {
            BoundedDequeIterator copy(*this);
            copy -= offset;
            return copy;
        }

        template<class DequeType, class T>
        std::ptrdiff_t BoundedDequeIterator<DequeType, T>::operator-(BoundedDequeIterator other) const
        {
            return mIndex - other.mIndex;
        }

        template<class DequeType, class T>
        template<class DequeType2, class T2>
        bool BoundedDequeIterator<DequeType, T>::operator==(const BoundedDequeIterator<DequeType2, T2>& other) const
        {
            really_assert(mDeque == other.mDeque);
            return mIndex == other.mIndex;
        }

        template<class DequeType, class T>
        template<class DequeType2, class T2>
        bool BoundedDequeIterator<DequeType, T>::operator!=(const BoundedDequeIterator<DequeType2, T2>& other) const
        {
            return !(*this == other);
        }

        template<class DequeType, class T>
        template<class DequeType2, class T2>
        bool BoundedDequeIterator<DequeType, T>::operator<(const BoundedDequeIterator<DequeType2, T2>& other) const
        {
            really_assert(mDeque == other.mDeque);
            return mIndex < other.mIndex;
        }

        template<class DequeType, class T>
        template<class DequeType2, class T2>
        bool BoundedDequeIterator<DequeType, T>::operator>(const BoundedDequeIterator<DequeType2, T2>& other) const
        {
            return other < *this;
        }

        template<class DequeType, class T>
        template<class DequeType2, class T2>
        bool BoundedDequeIterator<DequeType, T>::operator<=(const BoundedDequeIterator<DequeType2, T2>& other) const
        {
            return !(other < *this);
        }

        template<class DequeType, class T>
        template<class DequeType2, class T2>
        bool BoundedDequeIterator<DequeType, T>::operator>=(const BoundedDequeIterator<DequeType2, T2>& other) const
        {
            return !(*this < other);
        }
    }
}

#endif
