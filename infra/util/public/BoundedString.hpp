#ifndef INFRA_BOUNDED_STRING_HPP
#define INFRA_BOUNDED_STRING_HPP

//  BoundedString is similar to std::string, except that it can contain a maximum number of characters

#include "infra/util/public/MemoryRange.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <string>

namespace infra
{
    template<class T>
    class BoundedStringBase;

    using BoundedString = BoundedStringBase<char>;
    using BoundedConstString = BoundedStringBase<const char>;

    template<class T>
    class BoundedStringBase
    {
    public:
        typedef typename std::remove_const<T>::type NonConstT;

        template<std::size_t Max>
            using WithStorage = infra::WithStorage<BoundedStringBase, std::array<NonConstT, Max>>;

        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    public:
        static const size_type npos = -1;

    public:
        BoundedStringBase();
        BoundedStringBase(MemoryRange<NonConstT> range);
        BoundedStringBase(MemoryRange<NonConstT> range, size_type count, char ch);
        BoundedStringBase(MemoryRange<NonConstT> range, const BoundedStringBase& other, size_type pos, size_type count = BoundedStringBase::npos);
        BoundedStringBase(MemoryRange<NonConstT> range, const char* s, size_type count);
        BoundedStringBase(MemoryRange<NonConstT> range, const char* s);
        BoundedStringBase(MemoryRange<NonConstT> range, const std::string& s);
        BoundedStringBase(T* s, size_type count);
        BoundedStringBase(T* s);
        template<class InputIterator>
            BoundedStringBase(MemoryRange<NonConstT> range, InputIterator first, InputIterator last);
        BoundedStringBase(MemoryRange<NonConstT> range, std::initializer_list<char> initializerList);
        BoundedStringBase(MemoryRange<NonConstT> range, const BoundedStringBase& other);
        BoundedStringBase(const BoundedStringBase& other);
        template<class U>
            BoundedStringBase(const BoundedStringBase<U>& other);

        BoundedStringBase& operator=(const BoundedStringBase& other);
        BoundedStringBase& operator=(const char* s);
        BoundedStringBase& operator=(char ch);
        BoundedStringBase& operator=(const std::string& s);
        template<class U>
            void AssignFromStorage(const BoundedStringBase<U>& other);
        void AssignFromStorage(const char* s);
        void AssignFromStorage(char ch);
        void AssignFromStorage(const std::string& s);

        BoundedStringBase& assign(size_type count, char ch);
        template<class U>
            BoundedStringBase& assign(const BoundedStringBase<U>& other);
        template<class U>
            BoundedStringBase& assign(const BoundedStringBase<U>& other, size_type pos, size_type count);
        BoundedStringBase& assign(const char* s, size_type count);
        BoundedStringBase& assign(const char* s);
        BoundedStringBase& assign(const std::string& s);
        template<class InputIterator>
            BoundedStringBase& assign(InputIterator first, InputIterator last);

    public:
        reference operator[](size_type pos);
        const_reference operator[](size_type pos) const;
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;
        pointer data() const;

    public:
        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;
        iterator end();
        const_iterator end() const;
        const_iterator cend() const;
        reverse_iterator rbegin();
        const_reverse_iterator rbegin() const;
        const_reverse_iterator crbegin() const;
        reverse_iterator rend();
        const_reverse_iterator rend() const;
        const_reverse_iterator crend() const;

    public:
        bool empty() const;
        bool full() const;
        size_type size() const;
        size_type max_size() const;

    public:
        void clear();

        BoundedStringBase& insert(size_type index, size_type count, char ch);
        BoundedStringBase& insert(size_type index, const char* s);
        BoundedStringBase& insert(size_type index, const char* s, size_type count);
        BoundedStringBase& insert(size_type index, const BoundedStringBase& other);
        BoundedStringBase& insert(size_type index, const BoundedStringBase& other, size_type index_str, size_type count);
        BoundedStringBase& insert(size_type index, const std::string& other);
        BoundedStringBase& insert(size_type index, const std::string& other, size_type index_str, size_type count);
        iterator insert(const_iterator pos, char ch);
        iterator insert(const_iterator pos, size_type count, char ch);
        template<class InputIterator>
            iterator insert(const_iterator i, InputIterator first, InputIterator last);

        BoundedStringBase& erase(size_type index = 0, size_type count = npos);
        iterator erase(const_iterator position);
        iterator erase(const_iterator first, const_iterator last);

        void push_back(char ch);
        void pop_back();

        BoundedStringBase& append(size_type count, char ch);
        template<class U>
            BoundedStringBase& append(const BoundedStringBase<U>& other);
        template<class U>
            BoundedStringBase& append(const BoundedStringBase<U>& other, size_type pos, size_type count);
        BoundedStringBase& append(const char* s, size_type count);
        BoundedStringBase& append(const char* s);
        BoundedStringBase& append(const std::string& s);
        template<class InputIterator>
            BoundedStringBase& append(InputIterator first, InputIterator last);

        template<class U>
            BoundedStringBase& operator+=(const BoundedStringBase<U>& other);
        BoundedStringBase& operator+=(char ch);
        BoundedStringBase& operator+=(const char* s);
        BoundedStringBase& operator+=(const std::string& s);

        int compare(const BoundedStringBase& other) const;
        int compare(size_type pos1, size_type count1, const BoundedStringBase& other) const;
        int compare(size_type pos1, size_type count1, const BoundedStringBase& other, size_type pos2, size_type count2) const;
        int compare(const char* s) const;
        int compare(size_type pos1, size_type count1, const char* s) const;
        int compare(size_type pos1, size_type count1, const char* s, size_type count2) const;
        int compare(const std::string& s) const;
        int compare(size_type pos1, size_type count1, const std::string& s) const;
        int compare(size_type pos1, size_type count1, const std::string& s, size_type count2) const;

        BoundedStringBase& replace(size_type pos, size_type count, const BoundedStringBase& other);
        BoundedStringBase& replace(const_iterator first, const_iterator last, const BoundedStringBase& other);
        BoundedStringBase& replace(size_type pos, size_type count, const BoundedStringBase& other, size_type pos2, size_type count2);
        template<class InputIterator>
            BoundedStringBase& replace(const_iterator first, const_iterator last, InputIterator first2, InputIterator last2);
        BoundedStringBase& replace(size_type pos, size_type count, const char* cstr, size_type count2);
        BoundedStringBase& replace(const_iterator first, const_iterator last, const char* cstr, size_type count2);
        BoundedStringBase& replace(size_type pos, size_type count, const char* cstr);
        BoundedStringBase& replace(const_iterator first, const_iterator last, const char* cstr);
        BoundedStringBase& replace(size_type pos, size_type count, const std::string& s, size_type count2);
        BoundedStringBase& replace(const_iterator first, const_iterator last, const std::string& s, size_type count2);
        BoundedStringBase& replace(size_type pos, size_type count, const std::string& s);
        BoundedStringBase& replace(const_iterator first, const_iterator last, const std::string& s);
        BoundedStringBase& replace(size_type pos, size_type count, size_type count2, char ch);
        BoundedStringBase& replace(const_iterator first, const_iterator last, size_type count2, char ch);

    public:
        BoundedStringBase substr(size_type pos = 0, size_type count = npos);
        size_type copy(char* dest, size_type count, size_type pos = 0);
        void resize(size_type count);
        void resize(size_type count, char ch);
        void swap(BoundedStringBase& other);

    public:
        size_type find(const BoundedStringBase& other, size_type pos = 0) const;
        size_type find(const char* s, size_type pos, size_type count) const;
        size_type find(const char* s, size_type pos = 0) const;
        size_type find(char ch, size_type pos = 0) const;

        size_type rfind(const BoundedStringBase& other, size_type pos = npos) const;
        size_type rfind(const char* s, size_type pos, size_type count) const;
        size_type rfind(const char* s, size_type pos = npos) const;
        size_type rfind(char ch, size_type pos = npos) const;

        size_type find_first_of(const BoundedStringBase& other, size_type pos = 0) const;
        size_type find_first_of(const char* s, size_type pos, size_type count) const;
        size_type find_first_of(const char* s, size_type pos = 0) const;
        size_type find_first_of(char ch, size_type pos = 0) const;

        size_type find_first_not_of(const BoundedStringBase& other, size_type pos = 0) const;
        size_type find_first_not_of(const char* s, size_type pos, size_type count) const;
        size_type find_first_not_of(const char* s, size_type pos = 0) const;
        size_type find_first_not_of(char ch, size_type pos = 0) const;

        size_type find_last_of(const BoundedStringBase& other, size_type pos = npos) const;
        size_type find_last_of(const char* s, size_type pos, size_type count) const;
        size_type find_last_of(const char* s, size_type pos = npos) const;
        size_type find_last_of(char ch, size_type pos = npos) const;

        size_type find_last_not_of(const BoundedStringBase& other, size_type pos = npos) const;
        size_type find_last_not_of(const char* s, size_type pos, size_type count) const;
        size_type find_last_not_of(const char* s, size_type pos = npos) const;
        size_type find_last_not_of(char ch, size_type pos = npos) const;

    private:
        void MoveUp(size_type start, size_type count);
        void MoveDown(size_type start, size_type count);
        int CompareImpl(const char* begin1, const char* end1, const char* begin2, const char* end2) const;
        void ReplaceImpl(char* begin1, size_type count1, const char* begin2, size_type count2);
        void ReplaceImpl(char* begin1, size_type count1, char ch, size_type count2);

    private:
        template<class U>
            friend class BoundedStringBase;

        MemoryRange<NonConstT> range;
        size_type length = 0;
    };

    template<class T, class U>
        bool operator==(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs);
    template<class T, class U>
        bool operator!=(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs);
    template<class T, class U>
        bool operator<(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs);
    template<class T, class U>
        bool operator<=(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs);
    template<class T, class U>
        bool operator>(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs);
    template<class T, class U>
        bool operator>=(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs);
    template<class T>
        bool operator==(const char* lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator==(const BoundedStringBase<T>& lhs, const char* rhs);
    template<class T>
        bool operator==(const std::string& lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator==(const BoundedStringBase<T>& lhs, const std::string& rhs);
    template<class T>
        bool operator!=(const char* lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator!=(const BoundedStringBase<T>& lhs, const char* rhs);
    template<class T>
        bool operator!=(const std::string& lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator!=(const BoundedStringBase<T>& lhs, const std::string& rhs);
    template<class T>
        bool operator<(const char* lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator<(const BoundedStringBase<T>& lhs, const char* rhs);
    template<class T>
        bool operator<(const std::string& lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator<(const BoundedStringBase<T>& lhs, const std::string& rhs);
    template<class T>
        bool operator<=(const char* lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator<=(const BoundedStringBase<T>& lhs, const char* rhs);
    template<class T>
        bool operator<=(const std::string& lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator<=(const BoundedStringBase<T>& lhs, const std::string& rhs);
    template<class T>
        bool operator>(const char* lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator>(const BoundedStringBase<T>& lhs, const char* rhs);
    template<class T>
        bool operator>(const std::string& lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator>(const BoundedStringBase<T>& lhs, const std::string& rhs);
    template<class T>
        bool operator>=(const char* lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator>=(const BoundedStringBase<T>& lhs, const char* rhs);
    template<class T>
        bool operator>=(const std::string& lhs, const BoundedStringBase<T>& rhs);
    template<class T>
        bool operator>=(const BoundedStringBase<T>& lhs, const std::string& rhs);

    template<class T>
        void swap(BoundedStringBase<T>& lhs, BoundedStringBase<T>& rhs);

#ifdef _MSC_VER
    // gtest uses PrintTo to display the contents of BoundedStringBase<T>
    template<class T>
    void PrintTo(const BoundedStringBase<T>& string, std::ostream* os)
    {
        *os << '\"';
        for (char c : string)
            *os << c;
        *os << '\"';
    }

    template<class T, std::size_t Max>
    void PrintTo(const BoundedStringBase<T>::WithStorage<Max>& string, std::ostream* os)
    {
        *os << '\"';
        for (char c : string)
            *os << c;
        *os << '\"';
    }
#endif

    ////    Implementation    ////

    template<class T>
    BoundedStringBase<T>::BoundedStringBase()
    {}

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(MemoryRange<NonConstT> range)
        : range(range)
    {}

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(MemoryRange<NonConstT> range, size_type count, char ch)
        : range(range)
    {
        assign(count, ch);
    }

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(MemoryRange<NonConstT> range, const BoundedStringBase<T>& other, size_type pos, size_type count)
        : range(range)
    {
        assign(other, pos, count);
    }

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(MemoryRange<NonConstT> range, const char* s, size_type count)
        : range(range)
    {
        assign(s, count);
    }

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(MemoryRange<NonConstT> range, const char* s)
        : range(range)
    {
        assign(s);
    }

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(MemoryRange<NonConstT> range, const std::string& s)
        : range(range)
    {
        assign(s);
    }

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(T* s, size_type count)
        : range(const_cast<char*>(s), const_cast<char*>(s) + count)
        , length(count)
    {}

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(T* s)
        : range(const_cast<char*>(s), const_cast<char*>(s) + std::strlen(s))
        , length(range.size())
    {}

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(MemoryRange<NonConstT> range, std::initializer_list<char> initializerList)
        : range(range)
    {
        assign(initializerList.begin(), initializerList.end());
    }

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(MemoryRange<NonConstT> range, const BoundedStringBase<T>& other)
        : range(range)
    {
        assign(other);
    }

    template<class T>
    BoundedStringBase<T>::BoundedStringBase(const BoundedStringBase<T>& other)
        : range(other.range)
        , length(other.length)
    {}

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::operator=(const BoundedStringBase<T>& other)
    {
        range = other.range;
        length = other.length;
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::operator=(const char* s)
    {
        return assign(s);
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::operator=(char ch)
    {
        return assign(1, ch);
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::operator=(const std::string& s)
    {
        return assign(s);
    }

    template<class T>
    template<class U>
    void BoundedStringBase<T>::AssignFromStorage(const BoundedStringBase<U>& other)
    {
        assign(other);
    }

    template<class T>
    void BoundedStringBase<T>::AssignFromStorage(const char* s)
    {
        *this = s;
    }

    template<class T>
    void BoundedStringBase<T>::AssignFromStorage(char ch)
    {
        *this = ch;
    }

    template<class T>
    void BoundedStringBase<T>::AssignFromStorage(const std::string& s)
    {
        *this = s;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::assign(size_type count, char ch)
    {
        assert(count <= max_size());
        length = count;
        std::fill(range.begin(), range.begin() + length, ch);

        return *this;
    }

    template<class T>
    template<class U>
    BoundedStringBase<T>& BoundedStringBase<T>::assign(const BoundedStringBase<U>& other)
    {
        assert(other.size() <= max_size());
        length = other.length;
        std::copy(other.begin(), other.begin() + length, range.begin());

        return *this;
    }

    template<class T>
    template<class U>
    BoundedStringBase<T>& BoundedStringBase<T>::assign(const BoundedStringBase<U>& other, size_type pos, size_type count)
    {
        count = std::min(count, other.size());
        assert(count <= max_size());
        length = count;
        std::copy(other.range.begin() + pos, other.range.begin() + pos + length, range.begin());

        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::assign(const char* s, size_type count)
    {
        assert(count <= max_size());
        length = count;
        std::copy(s, s + length, range.begin());

        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::assign(const char* s)
    {
        return assign(s, std::strlen(s));
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::assign(const std::string& s)
    {
        return assign(s.data(), s.size());
    }

    template<class T>
    T& BoundedStringBase<T>::operator[](size_type pos)
    {
        return range[pos];
    }

    template<class T>
    const T& BoundedStringBase<T>::operator[](size_type pos) const
    {
        return range[pos];
    }

    template<class T>
    T& BoundedStringBase<T>::front()
    {
        return range.front();
    }

    template<class T>
    const T& BoundedStringBase<T>::front() const
    {
        return range.front();
    }

    template<class T>
    T& BoundedStringBase<T>::back()
    {
        return range[length - 1];
    }

    template<class T>
    const T& BoundedStringBase<T>::back() const
    {
        return range[length - 1];
    }

    template<class T>
    T* BoundedStringBase<T>::data() const
    {
        return range.begin();
    }

    template<class T>
    typename BoundedStringBase<T>::iterator BoundedStringBase<T>::begin()
    {
        return range.begin();
    }

    template<class T>
    typename BoundedStringBase<T>::const_iterator BoundedStringBase<T>::begin() const
    {
        return range.begin();
    }

    template<class T>
    typename BoundedStringBase<T>::const_iterator BoundedStringBase<T>::cbegin() const
    {
        return range.begin();
    }

    template<class T>
    typename BoundedStringBase<T>::iterator BoundedStringBase<T>::end()
    {
        return range.begin() + length;
    }

    template<class T>
    typename BoundedStringBase<T>::const_iterator BoundedStringBase<T>::end() const
    {
        return range.begin() + length;
    }

    template<class T>
    typename BoundedStringBase<T>::const_iterator BoundedStringBase<T>::cend() const
    {
        return range.begin() + length;
    }

    template<class T>
    typename BoundedStringBase<T>::reverse_iterator BoundedStringBase<T>::rbegin()
    {
        return reverse_iterator(range.begin() + length);
    }

    template<class T>
    typename BoundedStringBase<T>::const_reverse_iterator BoundedStringBase<T>::rbegin() const
    {
        return reverse_iterator(range.begin() + length);
    }

    template<class T>
    typename BoundedStringBase<T>::const_reverse_iterator BoundedStringBase<T>::crbegin() const
    {
        return reverse_iterator(range.begin() + length);
    }

    template<class T>
    typename BoundedStringBase<T>::reverse_iterator BoundedStringBase<T>::rend()
    {
        return reverse_iterator(range.begin());
    }

    template<class T>
    typename BoundedStringBase<T>::const_reverse_iterator BoundedStringBase<T>::rend() const
    {
        return reverse_iterator(range.begin());
    }

    template<class T>
    typename BoundedStringBase<T>::const_reverse_iterator BoundedStringBase<T>::crend() const
    {
        return reverse_iterator(range.begin());
    }

    template<class T>
    bool BoundedStringBase<T>::empty() const
    {
        return length == 0;
    }

    template<class T>
    bool BoundedStringBase<T>::full() const
    {
        return length == max_size();
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::size() const
    {
        return length;
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::max_size() const
    {
        return range.size();
    }

    template<class T>
    void BoundedStringBase<T>::clear()
    {
        length = 0;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::insert(size_type index, size_type count, char ch)
    {
        assert(length + count <= max_size());
        MoveUp(index, count);
        for (; count != 0; --count)
            range[index++] = ch;

        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::insert(size_type index, const char* s)
    {
        return insert(index, s, std::strlen(s));
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::insert(size_type index, const char* s, size_type count)
    {
        assert(length + count <= max_size());
        MoveUp(index, count);
        for (; count != 0; --count)
            range[index++] = *s++;

        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::insert(size_type index, const BoundedStringBase<T>& other)
    {
        assert(length + other.size() <= max_size());
        MoveUp(index, other.size());
        for (const_iterator i = other.begin(); i != other.end(); ++i)
            range[index++] = *i;

        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::insert(size_type index, const BoundedStringBase<T>& other, size_type index_str, size_type count)
    {
        assert(length + count <= max_size());
        MoveUp(index, count);
        const_iterator i = other.begin() + index_str;
        for (; count != 0; --count)
            range[index++] = *i++;

        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::insert(size_type index, const std::string& other)
    {
        insert(index, other.data(), other.size());
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::insert(size_type index, const std::string& other, size_type index_str, size_type count)
    {
        insert(index, other.data() + index_str, count);
    }

    template<class T>
    typename BoundedStringBase<T>::iterator BoundedStringBase<T>::insert(const_iterator pos, char ch)
    {
        assert(length != max_size());
        size_type index = pos - begin();
        MoveUp(index, 1);
        range[index++] = ch;

        return &range[index];
    }

    template<class T>
    typename BoundedStringBase<T>::iterator BoundedStringBase<T>::insert(const_iterator pos, size_type count, char ch)
    {
        assert(length + count <= max_size());
        size_type index = pos - begin();
        MoveUp(index, count);
        for (; count != 0; --count)
            range[index++] = ch;

        return &range[index];
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::erase(size_type index, size_type count)
    {
        count = std::min(count, length - index);
        MoveDown(index, count);
        return *this;
    }

    template<class T>
    typename BoundedStringBase<T>::iterator BoundedStringBase<T>::erase(const_iterator position)
    {
        MoveDown(position - begin(), 1);
        return begin() + (position - begin());
    }

    template<class T>
    typename BoundedStringBase<T>::iterator BoundedStringBase<T>::erase(const_iterator first, const_iterator last)
    {
        MoveDown(first - begin(), std::distance(first, last));
        return begin() + (first - begin());
    }

    template<class T>
    void BoundedStringBase<T>::push_back(char ch)
    {
        assert(length != max_size());
        range[length] = ch;
        ++length;
    }

    template<class T>
    void BoundedStringBase<T>::pop_back()
    {
        assert(length > 0);
        --length;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::append(size_type count, char ch)
    {
        assert(length + count <= max_size());
        for (; count > 0; --count, ++length)
            range[length] = ch;

        return *this;
    }

    template<class T>
    template<class U>
    BoundedStringBase<T>& BoundedStringBase<T>::append(const BoundedStringBase<U>& other)
    {
        assert(length + other.size() <= max_size());
        for (const_iterator i = other.begin(); i != other.end(); ++i, ++length)
            range[length] = *i;

        return *this;
    }

    template<class T>
    template<class U>
    BoundedStringBase<T>& BoundedStringBase<T>::append(const BoundedStringBase<U>& other, size_type pos, size_type count)
    {
        assert(length + count <= max_size());
        for (const_iterator i = other.begin() + pos; i != other.begin() + pos + count; ++i, ++length)
            range[length] = *i;

        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::append(const char* s, size_type count)
    {
        assert(length + count <= max_size());
        for (const char* i = s; i != s + count; ++i, ++length)
            range[length] = *i;

        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::append(const char* s)
    {
        return append(s, std::strlen(s));
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::append(const std::string& s)
    {
        return append(s.data(), s.size());
    }

    template<class T>
    template<class U>
    BoundedStringBase<T>& BoundedStringBase<T>::operator+=(const BoundedStringBase<U>& other)
    {
        return append(other);
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::operator+=(char ch)
    {
        return append(1, ch);
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::operator+=(const char* s)
    {
        return append(s);
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::operator+=(const std::string& s)
    {
        return append(s);
    }

    template<class T>
    int BoundedStringBase<T>::compare(const BoundedStringBase<T>& other) const
    {
        return CompareImpl(begin(), end(), other.begin(), other.end());
    }

    template<class T>
    int BoundedStringBase<T>::compare(size_type pos1, size_type count1, const BoundedStringBase<T>& other) const
    {
        return CompareImpl(begin() + pos1, begin() + pos1 + count1, other.begin(), other.end());
    }

    template<class T>
    int BoundedStringBase<T>::compare(size_type pos1, size_type count1, const BoundedStringBase<T>& other, size_type pos2, size_type count2) const
    {
        return CompareImpl(begin() + pos1, begin() + pos1 + count1, other.begin() + pos2, other.begin() + pos2 + count2);
    }

    template<class T>
    int BoundedStringBase<T>::compare(const char* s) const
    {
        return CompareImpl(begin(), end(), s, s + std::strlen(s));
    }

    template<class T>
    int BoundedStringBase<T>::compare(size_type pos1, size_type count1, const char* s) const
    {
        return CompareImpl(begin() + pos1, begin() + pos1 + count1, s, s + std::strlen(s));
    }

    template<class T>
    int BoundedStringBase<T>::compare(size_type pos1, size_type count1, const char* s, size_type count2) const
    {
        return CompareImpl(begin() + pos1, begin() + pos1 + count1, s, s + count2);
    }

    template<class T>
    int BoundedStringBase<T>::compare(const std::string& s) const
    {
        return CompareImpl(begin(), end(), s.data(), s.data() + s.size());
    }

    template<class T>
    int BoundedStringBase<T>::compare(size_type pos1, size_type count1, const std::string& s) const
    {
        return CompareImpl(begin() + pos1, begin() + pos1 + count1, s.data(), s.data() + s.size());
    }

    template<class T>
    int BoundedStringBase<T>::compare(size_type pos1, size_type count1, const std::string& s, size_type count2) const
    {
        return CompareImpl(begin() + pos1, begin() + pos1 + count1, s.data(), s.data() + count2);
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(size_type pos, size_type count, const BoundedStringBase<T>& other)
    {
        ReplaceImpl(begin() + pos, count, other.begin(), other.size());
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(const_iterator first, const_iterator last, const BoundedStringBase<T>& other)
    {
        ReplaceImpl(begin() + std::distance(cbegin(), first), std::distance(first, last), other.begin(), other.size());
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(size_type pos, size_type count, const BoundedStringBase<T>& other, size_type pos2, size_type count2)
    {
        ReplaceImpl(begin() + pos, count, other.begin() + pos2, count2);
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(size_type pos, size_type count, const char* cstr, size_type count2)
    {
        ReplaceImpl(begin() + pos, count, cstr, count2);
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(const_iterator first, const_iterator last, const char* cstr, size_type count2)
    {
        ReplaceImpl(begin() + std::distance(cbegin(), first), std::distance(first, last), cstr, count2);
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(size_type pos, size_type count, const char* cstr)
    {
        ReplaceImpl(begin() + pos, count, cstr, std::strlen(cstr));
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(const_iterator first, const_iterator last, const char* cstr)
    {
        ReplaceImpl(begin() + std::distance(cbegin(), first), std::distance(first, last), cstr, std::strlen(cstr));
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(size_type pos, size_type count, const std::string& s, size_type count2)
    {
        ReplaceImpl(begin() + pos, count, s.data(), count2);
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(const_iterator first, const_iterator last, const std::string& s, size_type count2)
    {
        ReplaceImpl(begin() + std::distance(cbegin(), first), std::distance(first, last), s.data(), count2);
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(size_type pos, size_type count, const std::string& s)
    {
        ReplaceImpl(begin() + pos, count, s.data(), s.size());
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(const_iterator first, const_iterator last, const std::string& s)
    {
        ReplaceImpl(begin() + std::distance(cbegin(), first), std::distance(first, last), s.data(), s.size());
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(size_type pos, size_type count, size_type count2, char ch)
    {
        ReplaceImpl(begin() + pos, count, ch, count2);
        return *this;
    }

    template<class T>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(const_iterator first, const_iterator last, size_type count2, char ch)
    {
        ReplaceImpl(begin() + std::distance(cbegin(), first), std::distance(first, last), ch, count2);
        return *this;
    }

    template<class T>
    BoundedStringBase<T> BoundedStringBase<T>::substr(size_type pos, size_type count)
    {
        assert(pos <= length);
        count = std::min(count, length - pos);
        BoundedStringBase<T> result(*this);
        result.range = MemoryRange<char>(const_cast<char*>(begin()) + pos, const_cast<char*>(begin()) + pos + count);
        result.length = result.range.size();
        return result;
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::copy(char* dest, size_type count, size_type pos)
    {
        assert(pos + count <= length);
        std::copy(begin() + pos, begin() + pos + count, dest);
        return count;
    }

    template<class T>
    void BoundedStringBase<T>::resize(size_type count)
    {
        resize(count, char());
    }

    template<class T>
    void BoundedStringBase<T>::resize(size_type count, char ch)
    {
        assert(count <= max_size());

        if (count > length)
            std::fill(end(), begin() + count, ch);

        length = count;
    }

    template<class T>
    void BoundedStringBase<T>::swap(BoundedStringBase<T>& other)
    {
        using std::swap;

        for (size_type i = 0; i < size() && i < other.size(); ++i)
            swap(range[i], other.range[i]);

        for (size_type i = size(); i < other.size(); ++i)
            range[i] = other.range[i];

        for (size_type i = other.size(); i < size(); ++i)
            other.range[i] = range[i];

        std::swap(length, other.length);
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find(const BoundedStringBase<T>& other, size_type pos) const
    {
        return find(other.begin(), pos, other.size());
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find(const char* s, size_type pos, size_type count) const
    {
        assert(pos <= length);
        for (const_iterator i = begin() + pos; i + count <= end(); ++i)
            if (CompareImpl(i, i + count, s, s + count) == 0)
                return i - begin();

        return npos;
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find(const char* s, size_type pos) const
    {
        return find(s, pos, std::strlen(s));
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find(char ch, size_type pos) const
    {
        return find(&ch, pos, 1);
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::rfind(const BoundedStringBase<T>& other, size_type pos) const
    {
        pos = std::min(pos, length);
        return rfind(other.begin(), pos, other.size());
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::rfind(const char* s, size_type pos, size_type count) const
    {
        pos = std::min(pos, length);
        for (const_iterator i = std::min(begin() + pos, end() - count); i >= begin(); --i)
            if (CompareImpl(i, i + count, s, s + count) == 0)
                return i - begin();

        return npos;
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::rfind(const char* s, size_type pos) const
    {
        return rfind(s, pos, std::strlen(s));
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::rfind(char ch, size_type pos) const
    {
        return rfind(&ch, pos, 1);
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_first_of(const BoundedStringBase<T>& other, size_type pos) const
    {
        return find_first_of(other.begin(), pos, other.size());
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_first_of(const char* s, size_type pos, size_type count) const
    {
        pos = std::min(pos, length);
        for (const_iterator i = begin() + pos; i != end(); ++i)
            if (std::find(s, s + count, *i) != s + count)
                return i - begin();

        return npos;
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_first_of(const char* s, size_type pos) const
    {
        return find_first_of(s, pos, std::strlen(s));
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_first_of(char ch, size_type pos) const
    {
        return find_first_of(&ch, pos, 1);
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_first_not_of(const BoundedStringBase<T>& other, size_type pos) const
    {
        return find_first_not_of(other.begin(), pos, other.size());
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_first_not_of(const char* s, size_type pos, size_type count) const
    {
        pos = std::min(pos, length);
        for (const_iterator i = begin() + pos; i != end(); ++i)
            if (std::find(s, s + count, *i) == s + count)
                return i - begin();

        return npos;
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_first_not_of(const char* s, size_type pos) const
    {
        return find_first_not_of(s, pos, std::strlen(s));
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_first_not_of(char ch, size_type pos) const
    {
        return find_first_not_of(&ch, pos, 1);
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_last_of(const BoundedStringBase<T>& other, size_type pos) const
    {
        return find_last_of(other.begin(), pos, other.size());
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_last_of(const char* s, size_type pos, size_type count) const
    {
        pos = std::min(pos, length);
        for (const_iterator i = begin() + pos - 1; i >= begin(); --i)
            if (std::find(s, s + count, *i) != s + count)
                return i - begin();

        return npos;
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_last_of(const char* s, size_type pos) const
    {
        return find_last_of(s, pos, std::strlen(s));
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_last_of(char ch, size_type pos) const
    {
        return find_last_of(&ch, pos, 1);
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_last_not_of(const BoundedStringBase<T>& other, size_type pos) const
    {
        return find_last_not_of(other.begin(), pos, other.size());
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_last_not_of(const char* s, size_type pos, size_type count) const
    {
        pos = std::min(pos, length);
        for (const_iterator i = begin() + pos - 1; i >= begin(); --i)
            if (std::find(s, s + count, *i) == s + count)
                return i - begin();

        return npos;
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_last_not_of(const char* s, size_type pos) const
    {
        return find_last_not_of(s, pos, std::strlen(s));
    }

    template<class T>
    typename BoundedStringBase<T>::size_type BoundedStringBase<T>::find_last_not_of(char ch, size_type pos) const
    {
        return find_last_not_of(&ch, pos, 1);
    }

    template<class T>
    void BoundedStringBase<T>::MoveUp(size_type start, size_type count)
    {
        std::memmove(range.begin() + start + count, range.begin() + start, length - start);
        length += count;
    }

    template<class T>
    void BoundedStringBase<T>::MoveDown(size_type start, size_type count)
    {
        std::memmove(range.begin() + start, range.begin() + start + count, length - start - count);
        length -= count;
    }

    template<class T>
    int BoundedStringBase<T>::CompareImpl(const char* begin1, const char* end1, const char* begin2, const char* end2) const
    {
        for (; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
            if (*begin1 != *begin2)
                return *begin1 < *begin2 ? -1 : 1;

        if (begin1 != end1)
            return 1;
        if (begin2 != end2)
            return -1;
        return 0;
    }

    template<class T>
    void BoundedStringBase<T>::ReplaceImpl(char* begin1, size_type count1, const char* begin2, size_type count2)
    {
        assert(length - count1 + count2 <= max_size());

        std::memmove(begin1 + count2, begin1 + count1, length - count1 - (begin() - begin1));
        std::memmove(begin1, begin2, count2);
        length += count2 - count1;
    }

    template<class T>
    void BoundedStringBase<T>::ReplaceImpl(char* begin1, size_type count1, char ch, size_type count2)
    {
        assert(length - count1 + count2 <= max_size());

        std::memmove(begin1 + count2, begin1 + count1, length - count1 - (begin() - begin1));
        std::fill(begin1, begin1 + count2, ch);
        length += count2 - count1;
    }

    template<class T>
    template<class U>
    BoundedStringBase<T>::BoundedStringBase(const BoundedStringBase<U>& other)
        : range(other.range)
        , length(other.length)
    {
        T x = {};    // const char
        U y;    // char
        y = x;  // Test to see that this constructor is used for assigning a non-const string to a const string
        (void)y;
    }

    template<class T>
    template<class InputIterator>
    BoundedStringBase<T>::BoundedStringBase(MemoryRange<NonConstT> range, InputIterator first, InputIterator last)
        : range(range)
    {
        assign(first, last);
    }

    template<class T>
    template<class InputIterator>
    BoundedStringBase<T>& BoundedStringBase<T>::assign(InputIterator first, InputIterator last)
    {
        for (length = 0; length != max_size() && first != last; ++length, ++first)
            range[length] = *first;

        return *this;
    }

    template<class T>
    template<class InputIterator>
    typename BoundedStringBase<T>::iterator BoundedStringBase<T>::insert(const_iterator i, InputIterator first, InputIterator last)
    {
        std::size_t additional = std::min(max_size() - length, static_cast<size_type>(std::distance(first, last)));
        size_type index = i - begin();
        MoveUp(index, additional);
        for (; additional != 0; --additional)
            range[index++] = *first++;

        return &range[index];
    }

    template<class T>
    template<class InputIterator>
    BoundedStringBase<T>& BoundedStringBase<T>::append(InputIterator first, InputIterator last)
    {
        for (InputIterator i = first; i != last && length != max_size(); ++i, ++length)
            range[length] = *i;

        return *this;
    }

    template<class T>
    template<class InputIterator>
    BoundedStringBase<T>& BoundedStringBase<T>::replace(const_iterator first, const_iterator last, InputIterator first2, InputIterator last2)
    {
        size_type count = first - begin();
        difference_type count2 = std::distance(first2, last2);
        size_type replacementSize = count2 - std::max<difference_type>(length - count + count2 - max_size(), 0);
        ReplaceImpl(begin() + count, last - first, first2, replacementSize);
        return *this;
    }

    template<class T, class U>
    bool operator==(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs)
    {
        return lhs.size() == rhs.size()
            && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<class T, class U>
    bool operator!=(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs)
    {
        return !(lhs == rhs);
    }

    template<class T, class U>
    bool operator<(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<class T, class U>
    bool operator<=(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs)
    {
        return !(rhs < lhs);
    }

    template<class T, class U>
    bool operator>(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs)
    {
        return rhs < lhs;
    }

    template<class T, class U>
    bool operator>=(const BoundedStringBase<T>& lhs, const BoundedStringBase<U>& rhs)
    {
        return !(lhs < rhs);
    }

    template<class T>
    bool operator==(const char* lhs, const BoundedStringBase<T>& rhs)
    {
        return std::strlen(lhs) == rhs.size()
            && std::equal(rhs.begin(), rhs.end(), lhs);
    }

    template<class T>
    bool operator==(const BoundedStringBase<T>& lhs, const char* rhs)
    {
        return lhs.size() == std::strlen(rhs)
            && std::equal(lhs.begin(), lhs.end(), rhs);
    }

    template<class T>
    bool operator==(const std::string& lhs, const BoundedStringBase<T>& rhs)
    {
        return lhs.size() == rhs.size()
            && std::equal(rhs.begin(), rhs.end(), lhs.begin());
    }

    template<class T>
    bool operator==(const BoundedStringBase<T>& lhs, const std::string& rhs)
    {
        return lhs.size() == rhs.size()
            && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<class T>
    bool operator!=(const char* lhs, const BoundedStringBase<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template<class T>
    bool operator!=(const BoundedStringBase<T>& lhs, const char* rhs)
    {
        return !(lhs == rhs);
    }

    template<class T>
    bool operator!=(const std::string& lhs, const BoundedStringBase<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template<class T>
    bool operator!=(const BoundedStringBase<T>& lhs, const std::string& rhs)
    {
        return !(lhs == rhs);
    }

    template<class T>
    bool operator<(const char* lhs, const BoundedStringBase<T>& rhs)
    {
        return std::lexicographical_compare(lhs, lhs + std::strlen(lhs), rhs.begin(), rhs.end());
    }

    template<class T>
    bool operator<(const BoundedStringBase<T>& lhs, const char* rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs, rhs + std::strlen(rhs));
    }

    template<class T>
    bool operator<(const std::string& lhs, const BoundedStringBase<T>& rhs)
    {
        return std::lexicographical_compare(lhs.data(), lhs.data() + lhs.size(), rhs.begin(), rhs.end());
    }

    template<class T>
    bool operator<(const BoundedStringBase<T>& lhs, const std::string& rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.data(), rhs.data() + rhs.size());
    }

    template<class T>
    bool operator<=(const char* lhs, const BoundedStringBase<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template<class T>
    bool operator<=(const BoundedStringBase<T>& lhs, const char* rhs)
    {
        return !(rhs < lhs);
    }

    template<class T>
    bool operator<=(const std::string& lhs, const BoundedStringBase<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template<class T>
    bool operator<=(const BoundedStringBase<T>& lhs, const std::string& rhs)
    {
        return !(rhs < lhs);
    }

    template<class T>
    bool operator>(const char* lhs, const BoundedStringBase<T>& rhs)
    {
        return rhs < lhs;
    }

    template<class T>
    bool operator>(const BoundedStringBase<T>& lhs, const char* rhs)
    {
        return rhs < lhs;
    }

    template<class T>
    bool operator>(const std::string& lhs, const BoundedStringBase<T>& rhs)
    {
        return rhs < lhs;
    }

    template<class T>
    bool operator>(const BoundedStringBase<T>& lhs, const std::string& rhs)
    {
        return rhs < lhs;
    }

    template<class T>
    bool operator>=(const char* lhs, const BoundedStringBase<T>& rhs)
    {
        return !(lhs < rhs);
    }

    template<class T>
    bool operator>=(const BoundedStringBase<T>& lhs, const char* rhs)
    {
        return !(lhs < rhs);
    }

    template<class T>
    bool operator>=(const std::string& lhs, const BoundedStringBase<T>& rhs)
    {
        return !(lhs < rhs);
    }

    template<class T>
    bool operator>=(const BoundedStringBase<T>& lhs, const std::string& rhs)
    {
        return !(lhs < rhs);
    }

    template<class T>
    void swap(BoundedStringBase<T>& lhs, BoundedStringBase<T>& rhs)
    {
        lhs.swap(rhs);
    }
}

#endif
