#ifndef INFRA_INTRUSIVE_LIST_HPP
#define INFRA_INTRUSIVE_LIST_HPP

#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <type_traits>

namespace infra
{
    namespace detail
    {
        template<class T>
        struct IntrusiveListNode
        {
            IntrusiveListNode();
            IntrusiveListNode(const IntrusiveListNode& other);

            IntrusiveListNode& operator=(const IntrusiveListNode& other);

            IntrusiveListNode<T>* next;
            IntrusiveListNode<T>* previous;
        };

        template<class T>
        class IntrusiveListIterator;
    }

    template<class T>
    class IntrusiveList
    {
    public:
        typedef detail::IntrusiveListNode<T> NodeType;

        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef detail::IntrusiveListIterator<T> iterator;
        typedef detail::IntrusiveListIterator<const T> const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef typename std::iterator_traits<iterator>::difference_type difference_type;
        typedef std::size_t size_type;

    public:
        IntrusiveList();
        template<class InputIterator>
            IntrusiveList(InputIterator first, InputIterator last);
        IntrusiveList(const IntrusiveList&) = delete;
        IntrusiveList(IntrusiveList&& other);
        IntrusiveList& operator=(const IntrusiveList&) = delete;
        IntrusiveList& operator=(IntrusiveList&& other);
        ~IntrusiveList();

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
        bool empty() const;
        bool has_element(const_reference value) const;  // Runs in O(n) time

    public:
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;

    public:
        void push_front(const_reference value);
        void push_back(const_reference value);
        void pop_front();
        void pop_back();

        void insert(const_iterator position, const_reference value);
        void erase(const_reference value);

        template<class InputIterator>
            void Assign(InputIterator first, InputIterator last);

        void swap(IntrusiveList& other);

        void clear();

    public:
        bool operator==(const IntrusiveList& other) const;
        bool operator!=(const IntrusiveList& other) const;
        bool operator<(const IntrusiveList& other) const;
        bool operator<=(const IntrusiveList& other) const;
        bool operator>(const IntrusiveList& other) const;
        bool operator>=(const IntrusiveList& other) const;

    private:
        size_type mSize;
        NodeType* mBegin;
        NodeType mEnd;
    };

    template<class T>
        void swap(IntrusiveList<T>& x, IntrusiveList<T>& y);

    namespace detail
    {
        template<class T>
        class IntrusiveListIterator
            : public std::iterator<std::bidirectional_iterator_tag, T>
        {
        public:
            IntrusiveListIterator();
            IntrusiveListIterator(const IntrusiveListNode<typename std::remove_const<T>::type>* node);
            template<class T2>
                IntrusiveListIterator(const IntrusiveListIterator<T2>& other);

            template<class T2>
                IntrusiveListIterator& operator=(const IntrusiveListIterator<T2>& other);

            T& operator*() const;
            T* operator->() const;

            IntrusiveListIterator& operator++();
            IntrusiveListIterator operator++(int);
            IntrusiveListIterator& operator--();
            IntrusiveListIterator operator--(int);

            template<class T2>
                bool operator==(const IntrusiveListIterator<T2>& other) const;
            template<class T2>
                bool operator!=(const IntrusiveListIterator<T2>& other) const;

        private:
            template<class>
            friend class IntrusiveListIterator;
            template<class>
            friend class IntrusiveList;

            const IntrusiveListNode<typename std::remove_const<T>::type>* mNode;
        };
    }

    ////    Implementation    ////

    template<class T>
    IntrusiveList<T>::IntrusiveList()
        : mSize(0)
        , mBegin(&mEnd)
    {}

    template<class T>
    template<class InputIterator>
    IntrusiveList<T>::IntrusiveList(InputIterator first, InputIterator last)
        : mSize(0)
        , mBegin(&mEnd)
    {
        Assign(first, last);
    }

    template<class T>
    IntrusiveList<T>::IntrusiveList(IntrusiveList&& other)
        : mSize(0)
        , mBegin(&mEnd)
    {
        *this = std::move(other);
    }

    template<class T>
    IntrusiveList<T>& IntrusiveList<T>::operator=(IntrusiveList&& other)
    {
        clear();
        mSize = other.mSize;
        mBegin = other.mBegin;
        mEnd.previous = other.mEnd.previous;
        if (mEnd.previous)
            mEnd.previous->next = &mEnd;
        other.clear();

        return *this;
    }

    template<class T>
    IntrusiveList<T>::~IntrusiveList()
    {}

    template<class T>
    typename IntrusiveList<T>::iterator IntrusiveList<T>::begin()
    {
        return iterator(mBegin);
    }

    template<class T>
    typename IntrusiveList<T>::const_iterator IntrusiveList<T>::begin() const
    {
        return const_iterator(mBegin);
    }

    template<class T>
    typename IntrusiveList<T>::iterator IntrusiveList<T>::end()
    {
        return iterator(&mEnd);
    }

    template<class T>
    typename IntrusiveList<T>::const_iterator IntrusiveList<T>::end() const
    {
        return const_iterator(&mEnd);
    }

    template<class T>
    typename IntrusiveList<T>::reverse_iterator IntrusiveList<T>::rbegin()
    {
        return reverse_iterator(end());
    }

    template<class T>
    typename IntrusiveList<T>::const_reverse_iterator IntrusiveList<T>::rbegin() const
    {
        return const_reverse_iterator(end());
    }

    template<class T>
    typename IntrusiveList<T>::reverse_iterator IntrusiveList<T>::rend()
    {
        return reverse_iterator(begin());
    }

    template<class T>
    typename IntrusiveList<T>::const_reverse_iterator IntrusiveList<T>::rend() const
    {
        return const_reverse_iterator(begin());
    }

    template<class T>
    typename IntrusiveList<T>::const_iterator IntrusiveList<T>::cbegin() const
    {
        return begin();
    }

    template<class T>
    typename IntrusiveList<T>::const_iterator IntrusiveList<T>::cend() const
    {
        return end();
    }

    template<class T>
    typename IntrusiveList<T>::const_reverse_iterator IntrusiveList<T>::crbegin() const
    {
        return rbegin();
    }

    template<class T>
    typename IntrusiveList<T>::const_reverse_iterator IntrusiveList<T>::crend() const
    {
        return rend();
    }

    template<class T>
    typename IntrusiveList<T>::size_type IntrusiveList<T>::size() const
    {
        return mSize;
    }

    template<class T>
    bool IntrusiveList<T>::empty() const
    {
        return mSize == 0;
    }

    template<class T>
    bool IntrusiveList<T>::has_element(const_reference value) const
    {
        for (const_reference item: *this)
            if (&item == &value)
                return true;

        return false;
    }

    template<class T>
    typename IntrusiveList<T>::reference IntrusiveList<T>::front()
    {
        return static_cast<T&>(*mBegin);
    }

    template<class T>
    typename IntrusiveList<T>::const_reference IntrusiveList<T>::front() const
    {
        return static_cast<T&>(*mBegin);
    }

    template<class T>
    typename IntrusiveList<T>::reference IntrusiveList<T>::back()
    {
        return static_cast<T&>(*mEnd.previous);
    }

    template<class T>
    typename IntrusiveList<T>::const_reference IntrusiveList<T>::back() const
    {
        return static_cast<T&>(*mEnd.previous);
    }

    template<class T>
    void IntrusiveList<T>::push_front(const_reference value)
    {
        NodeType& node = const_cast<reference>(value);
        node.next = mBegin;
        node.previous = nullptr;

        if (node.next)
            node.next->previous = &node;

        mBegin = &node;
        ++mSize;
    }

    template<class T>
    void IntrusiveList<T>::push_back(const_reference value)
    {
        NodeType& node = const_cast<reference>(value);
        node.next = &mEnd;
        node.previous = mEnd.previous;

        if (node.previous)
            node.previous->next = &node;
        else
            mBegin = &node;

        mEnd.previous = &node;
        ++mSize;
    }

    template<class T>
    void IntrusiveList<T>::pop_front()
    {
        mBegin = mBegin->next;
        mBegin->previous = nullptr;
        --mSize;
    }

    template<class T>
    void IntrusiveList<T>::pop_back()
    {
        mEnd.previous = mEnd.previous->previous;
        if (mEnd.previous)
            mEnd.previous->next = &mEnd;
        --mSize;
    }

    template<class T>
    void IntrusiveList<T>::insert(const_iterator position, const_reference value)
    {
        NodeType& node = const_cast<reference>(value);
        node.previous = position.mNode->previous;
        node.next = const_cast<NodeType*>(position.mNode);
        const_cast<NodeType*>(position.mNode)->previous = &node;
        if (node.previous)
            node.previous->next = &node;
        else
            mBegin = &node;
        ++mSize;
    }

    template<class T>
    void IntrusiveList<T>::erase(const_reference value)
    {
        NodeType& node = const_cast<reference>(value);
        if (node.previous)
            node.previous->next = node.next;
        else
            mBegin = node.next;

        node.next->previous = node.previous;
        --mSize;
    }

    template<class T>
    template<class InputIterator>
    void IntrusiveList<T>::Assign(InputIterator first, InputIterator last)
    {
        clear();
        for (; first != last; ++first)
            push_back(*first);
    }

    template<class T>
    void IntrusiveList<T>::swap(IntrusiveList& other)
    {
        using std::swap;
        std::swap(mEnd, other.mEnd);
        std::swap(mBegin, other.mBegin);
        std::swap(mSize, other.mSize);
    }

    template<class T>
    void IntrusiveList<T>::clear()
    {
        mBegin = &mEnd;
        mEnd.previous = nullptr;
        mSize = 0;
    }

    template<class T>
    bool IntrusiveList<T>::operator==(const IntrusiveList& other) const
    {
        return size() == other.size()
            && std::equal(begin(), end(), other.begin());
    }

    template<class T>
    bool IntrusiveList<T>::operator!=(const IntrusiveList& other) const
    {
        return !(*this == other);
    }

    template<class T>
    bool IntrusiveList<T>::operator<(const IntrusiveList& other) const
    {
        return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }

    template<class T>
    bool IntrusiveList<T>::operator<=(const IntrusiveList& other) const
    {
        return !(other < *this);
    }

    template<class T>
    bool IntrusiveList<T>::operator>(const IntrusiveList& other) const
    {
        return other < *this;
    }

    template<class T>
    bool IntrusiveList<T>::operator>=(const IntrusiveList& other) const
    {
        return !(*this < other);
    }

    template<class T>
    void swap(IntrusiveList<T>& x, IntrusiveList<T>& y)
    {
        x.swap(y);
    }

    namespace detail
    {
        template<class T>
        IntrusiveListNode<T>::IntrusiveListNode()
            : next(nullptr)
            , previous(nullptr)
        {}

        template<class T>
        IntrusiveListNode<T>::IntrusiveListNode(const IntrusiveListNode& other)
            : next(nullptr)
            , previous(nullptr)
        {}

        template<class T>
        IntrusiveListNode<T>& IntrusiveListNode<T>::operator=(const IntrusiveListNode& other)
        {
            return *this;
        }

        template<class T>
        IntrusiveListIterator<T>::IntrusiveListIterator()
            : mNode(nullptr)
        {}

        template<class T>
        IntrusiveListIterator<T>::IntrusiveListIterator(const IntrusiveListNode<typename std::remove_const<T>::type>* node)
            : mNode(node)
        {}

        template<class T>
        template<class T2>
        IntrusiveListIterator<T>::IntrusiveListIterator(const IntrusiveListIterator<T2>& other)
            : mNode(other.mNode)
        {}

        template<class T>
        template<class T2>
        IntrusiveListIterator<T>& IntrusiveListIterator<T>::operator=(const IntrusiveListIterator<T2>& other)
        {
            mNode = other.mNode;

            return *this;
        }

        template<class T>
        T& IntrusiveListIterator<T>::operator*() const
        {
            return const_cast<T&>(static_cast<const T&>(*mNode));
        }

        template<class T>
        T* IntrusiveListIterator<T>::operator->() const
        {
            return const_cast<T*>(static_cast<const T*>(mNode));
        }

        template<class T>
        IntrusiveListIterator<T>& IntrusiveListIterator<T>::operator++()
        {
            mNode = mNode->next;
            return *this;
        }

        template<class T>
        IntrusiveListIterator<T> IntrusiveListIterator<T>::operator++(int)
        {
            IntrusiveListIterator copy(*this);
            mNode = mNode->next;
            return copy;
        }

        template<class T>
        IntrusiveListIterator<T>& IntrusiveListIterator<T>::operator--()
        {
            mNode = mNode->previous;
            return *this;
        }

        template<class T>
        IntrusiveListIterator<T> IntrusiveListIterator<T>::operator--(int)
        {
            IntrusiveListIterator copy(*this);
            mNode = mNode->previous;
            return copy;
        }

        template<class T>
        template<class T2>
        bool IntrusiveListIterator<T>::operator==(const IntrusiveListIterator<T2>& other) const
        {
            return mNode == other.mNode;
        }

        template<class T>
        template<class T2>
        bool IntrusiveListIterator<T>::operator!=(const IntrusiveListIterator<T2>& other) const
        {
            return !(*this == other);
        }
    }

}

#endif
