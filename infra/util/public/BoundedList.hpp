#ifndef INFRA_BOUNDED_LIST_HPP
#define INFRA_BOUNDED_LIST_HPP

//  The BoundedList provides a doubly-linked list similar to std::list, however,
//  it is limited in the amount of values that can be pushed in the list. Moreover, nodes can
//  be moved from one std::list to another in constant time, but in BoundedList
//  this is done in linear time, since nodes are copied from one BoundedList to another.
//  Node storage is specifically coupled to a BoundedList.
//  The usecase for using BoundedList is that values can be inserted in the middle of
//  the list in constant time.

#include "infra/util/public/MemoryRange.hpp"
#include "infra/util/public/ReallyAssert.hpp"
#include "infra/util/public/StaticStorage.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <array>
#include <iterator>

namespace infra
{
    namespace detail
    {
        template<class T>
        struct BoundedListNode
        {
            StaticStorage<T> storage;
            BoundedListNode<T>* next;
            BoundedListNode<T>* previous;
        };

        template<class T>
        class BoundedListIterator;
    }

    template<class T>
    class BoundedList
    {
    public:
        template<std::size_t Max>
            using WithMaxSize = infra::WithStorage<BoundedList<T>, std::array<StaticStorage<detail::BoundedListNode<T>>, Max>>;

        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef detail::BoundedListIterator<T> iterator;
        typedef detail::BoundedListIterator<const T> const_iterator;
        typedef typename std::iterator_traits<iterator>::difference_type difference_type;
        typedef std::size_t size_type;

    public:
        BoundedList();
        BoundedList(const BoundedList& other) = delete;
        explicit BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage);
        BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, size_type n, const value_type& value = value_type());
        template<class InputIterator>
            BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, InputIterator first, InputIterator last);
        BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, std::initializer_list<T> initializerList);
        BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, const BoundedList& other);
        BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, BoundedList&& other);
        ~BoundedList();

        BoundedList& operator=(const BoundedList& other);
        BoundedList& operator=(BoundedList&& other);
        void AssignFromStorage(const BoundedList& other);
        void AssignFromStorage(BoundedList&& other);

    public:
        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;

    public:
        size_type size() const;
        size_type max_size() const;
        bool empty() const;
        bool full() const;

    public:
        value_type& front();
        const value_type& front() const;
        value_type& back();
        const value_type& back() const;

    public:
        void push_front(const value_type& value);
        void push_front(value_type&& value);
        void pop_front();

        void push_back(const value_type& value);
        void push_back(value_type&& value);
        void pop_back();

        void insert(iterator position, const value_type& value);
        void insert(iterator position, value_type&& value);

        template<class InputIterator>
            void assign(InputIterator first, InputIterator last);
        void assign(size_type n, const value_type& value);
        template<class InputIterator>
            void move_from_range(InputIterator first, InputIterator last);

        void erase(iterator position);
        void erase_all_after(iterator position);

        void swap(BoundedList& other);

        void clear();

        template<class... Args>
            void emplace_front(Args&&... args);
        template<class... Args>
            void emplace_back(Args&&... args);

    public:
        bool operator==(const BoundedList& other) const;
        bool operator!=(const BoundedList& other) const;
        bool operator<(const BoundedList& other) const;
        bool operator<=(const BoundedList& other) const;
        bool operator>(const BoundedList& other) const;
        bool operator>=(const BoundedList& other) const;

    private:
        typedef detail::BoundedListNode<typename std::remove_const<T>::type> NodeType;

    private:
        NodeType* AllocateNode();
        
    private:
        infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage;
        size_type numAllocated = 0;
        size_type numInitialized = 0;
        NodeType* firstNode = nullptr;
        NodeType* lastNode = nullptr;
        NodeType* freeList = nullptr;
    };

    template<class T>
        void swap(BoundedList<T>& x, BoundedList<T>& y);

    namespace detail
    {
        template<class T>
        class BoundedListIterator
            : public std::iterator<std::forward_iterator_tag, T>
        {
        private:
            typedef detail::BoundedListNode<typename std::remove_const<T>::type> NodeType;

        public:
            BoundedListIterator();
            BoundedListIterator(NodeType* node);
            template<class T2>
                BoundedListIterator(const BoundedListIterator<T2>& other);

            template<class T2>
                BoundedListIterator& operator=(const BoundedListIterator<T2>& other);

            T& operator*() const;
            T* operator->() const;

            NodeType& node();

            BoundedListIterator& operator++();
            BoundedListIterator operator++(int);
            BoundedListIterator& operator--();
            BoundedListIterator operator--(int);

            template<class T2>
                bool operator==(const BoundedListIterator<T2>& other) const;
            template<class T2>
                bool operator!=(const BoundedListIterator<T2>& other) const;

        private:
            template<class>
            friend class BoundedListIterator;

            NodeType* mNode;
        };
    }

    ////    Implementation    ////
 
    template<class T>
    BoundedList<T>::BoundedList()
    {}

    template<class T>
    BoundedList<T>::BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage)
        : storage(storage)
    {}

    template<class T>
    BoundedList<T>::BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, size_type n, const value_type& value)
        : storage(storage)
    {
        assign(n, value);
    }

    template<class T>
    template<class InputIterator>
    BoundedList<T>::BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, InputIterator first, InputIterator last)
        : storage(storage)
    {
        assign(first, last);
    }

    template<class T>
    BoundedList<T>::BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, std::initializer_list<T> initializerList)
        : storage(storage)
    {
        assign(initializerList.begin(), initializerList.end());
    }

    template<class T>
    BoundedList<T>::BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, const BoundedList& other)
        : storage(storage)
    {
        assign(other.begin(), other.end());
    }

    template<class T>
    BoundedList<T>::BoundedList(infra::MemoryRange<infra::StaticStorage<detail::BoundedListNode<T>>> storage, BoundedList&& other)
        : storage(storage)
    {
        move_from_range(other.begin(), other.end());
        other.clear();
    }

    template<class T>
    BoundedList<T>::~BoundedList()
    {
        clear();
    }

    template<class T>
    BoundedList<T>& BoundedList<T>::operator=(const BoundedList& other)
    {
        if (this != &other)
        {
            clear();

            assign(other.begin(), other.end());
        }

        return *this;
    }

    template<class T>
    BoundedList<T>& BoundedList<T>::operator=(BoundedList&& other)
    {
        clear();
        move_from_range(other.begin(), other.end());

        return *this;
    }

    template<class T>
    void BoundedList<T>::AssignFromStorage(const BoundedList& other)
    {
        *this = other;
    }

    template<class T>
    void BoundedList<T>::AssignFromStorage(BoundedList&& other)
    {
        *this = std::move(other);
    }

    template<class T>
    typename BoundedList<T>::iterator BoundedList<T>::begin()
    {
        return iterator(firstNode);
    }

    template<class T>
    typename BoundedList<T>::const_iterator BoundedList<T>::begin() const
    {
        return iterator(firstNode);
    }

    template<class T>
    typename BoundedList<T>::iterator BoundedList<T>::end()
    {
        return iterator();
    }

    template<class T>
    typename BoundedList<T>::const_iterator BoundedList<T>::end() const
    {
        return const_iterator();
    }

    template<class T>
    typename BoundedList<T>::const_iterator BoundedList<T>::cbegin() const
    {
        return const_iterator(firstNode);
    }

    template<class T>
    typename BoundedList<T>::const_iterator BoundedList<T>::cend() const
    {
        return const_iterator();
    }

    template<class T>
    typename BoundedList<T>::size_type BoundedList<T>::size() const
    {
        return numAllocated;
    }

    template<class T>
    typename BoundedList<T>::size_type BoundedList<T>::max_size() const
    {
        return storage.size();
    }

    template<class T>
    bool BoundedList<T>::empty() const
    {
        return numAllocated == 0;
    }

    template<class T>
    bool BoundedList<T>::full() const
    {
        return numAllocated == storage.size();
    }

    template<class T>
    typename BoundedList<T>::value_type& BoundedList<T>::front()
    {
        return *firstNode->storage;
    }

    template<class T>
    const typename BoundedList<T>::value_type& BoundedList<T>::front() const
    {
        return *firstNode->storage;
    }

    template<class T>
    typename BoundedList<T>::value_type& BoundedList<T>::back()
    {
        return *lastNode->storage;
    }

    template<class T>
    const typename BoundedList<T>::value_type& BoundedList<T>::back() const
    {
        return *lastNode->storage;
    }

    template<class T>
    void BoundedList<T>::push_front(const value_type& value)
    {
        NodeType* node = AllocateNode();
        node->storage.Construct(value);

        node->next = firstNode;
        node->previous = nullptr;
        firstNode = node;

        if (!node->next)
            lastNode = node;
        else
            node->next->previous = node;

        ++numAllocated;
    }

    template<class T>
    void BoundedList<T>::push_front(value_type&& value)
    {
        NodeType* node = AllocateNode();
        node->storage.Construct(std::move(value));

        node->next = firstNode;
        node->previous = nullptr;
        firstNode = node;

        if (!node->next)
            lastNode = node;
        else
            node->next->previous = node;

        ++numAllocated;
    }

    template<class T>
    void BoundedList<T>::pop_front()
    {
        firstNode->storage.Destruct();
            
        NodeType* oldStart = firstNode;
        firstNode = firstNode->next;

        if (firstNode)
            firstNode->previous = nullptr;
        else
            lastNode = nullptr;

        oldStart->next = freeList;
        freeList = oldStart;

        --numAllocated;
    }

    template<class T>
    void BoundedList<T>::push_back(const value_type& value)
    {
        NodeType* node = AllocateNode();
        node->storage.Construct(value);

        node->previous = lastNode;
        node->next = nullptr;
        lastNode = node;

        if (!node->previous)
            firstNode = node;
        else
            node->previous->next = node;

        ++numAllocated;
    }

    template<class T>
    void BoundedList<T>::push_back(value_type&& value)
    {
        NodeType* node = AllocateNode();
        node->storage.Construct(std::move(value));

        node->previous = lastNode;
        node->next = nullptr;
        lastNode = node;

        if (!node->previous)
            firstNode = node;
        else
            node->previous->next = node;

        ++numAllocated;
    }

    template<class T>
    void BoundedList<T>::pop_back()
    {
        firstNode->storage.Destruct();

        NodeType* oldEnd = lastNode;
        lastNode = lastNode->previous;

        if (lastNode)
            lastNode->next = nullptr;
        else
            firstNode = nullptr;

        oldEnd->next = freeList;
        freeList = oldEnd;

        --numAllocated;
    }

    template<class T>
    void BoundedList<T>::insert(iterator position, const value_type& value)
    {
        NodeType* node = AllocateNode();
        node->storage.Construct(value);

        if (position != end())
        {
            node->next = &position.node();
            node->previous = position.node().previous;

            if (node->previous)
                node->previous->next = node;
            else
                firstNode = node;

            position.node().previous = node;
        }
        else
            push_back(value);

        ++numAllocated;
    }

    template<class T>
    void BoundedList<T>::insert(iterator position, value_type&& value)
    {
        NodeType* node = AllocateNode();
        node->storage.Construct(std::move(value));

        if (position != end())
        {
            node->next = &position.node();
            node->previous = position.node().previous;

            if (node->previous)
                node->previous->next = node;
            else
                firstNode = node;

            position.node().previous = node;
        }
        else
            push_back(std::move(value));

        ++numAllocated;
    }

    template<class T>
    template<class InputIterator>
    void BoundedList<T>::assign(InputIterator first, InputIterator last)
    {
        clear();

        while (first != last)
        {
            push_back(*first);
            ++first;
        }
    }

    template<class T>
    void BoundedList<T>::assign(size_type n, const value_type& value)
    {
        clear();

        for (size_type i = 0; i != n; ++i)
            push_back(value);
    }

    template<class T>
    template<class InputIterator>
    void BoundedList<T>::move_from_range(InputIterator first, InputIterator last)
    {
        clear();

        while (first != last)
        {
            push_back(std::move(*first));
            ++first;
        }
    }

    template<class T>
    void BoundedList<T>::erase(iterator position)
    {
        NodeType* node = &position.node();
        NodeType* next = node->next;
        NodeType* previous = node->previous;

        if (next)
            next->previous = previous;
        else
            lastNode = previous;

        if (previous)
            previous->next = next;
        else
            firstNode = next;

        node->next = freeList;
        freeList = node;

        --numAllocated;
    }

    template<class T>
    void BoundedList<T>::erase_all_after(iterator position)
    {
        NodeType* node = position.node().next;
        position.node().next = nullptr;

        lastNode = node;

        while (node)
        {
            node->storage.Destruct();
            NodeType* oldFreeList = freeList;
            freeList = node;
            node = node->next;
            freeList->next = oldFreeList;

            --numAllocated;
        }
    }

    template<class T>
    void BoundedList<T>::swap(BoundedList& other)
    {
        using std::swap;

        iterator i1 = begin();
        iterator i2 = other.begin();
        iterator i1old;
        iterator i2old;

        while (i1 != end() && i2 != other.end())
        {
            swap(*i1, *i2);
            i1old = i1;
            i2old = i2;
            ++i1;
            ++i2;
        }

        if (i1 != end())
        {
            while (i1 != end())
            {
                other.push_back(std::move(*i1));
                ++i1;
            }

            erase_all_after(i1old);
        }

        if (i2 != other.end())
        {
            while (i2 != other.end())
            {
                push_back(std::move(*i2));
                ++i2;
            }

            other.erase_all_after(i2old);
        }
    }

    template<class T>
    void BoundedList<T>::clear()
    {
        while (lastNode)
            pop_back();
    }

    template<class T>
    template<class... Args>
    void BoundedList<T>::emplace_front(Args&&... args)
    {
        NodeType* node = AllocateNode();
        node->storage.Construct(std::forward<Args>(args)...);

        node->next = firstNode;
        node->previous = nullptr;

        if (node->next)
            node->next->previous = node;
        else
            lastNode = node;

        firstNode = node;

        ++numAllocated;
    }

    template<class T>
    template<class... Args>
    void BoundedList<T>::emplace_back(Args&&... args)
    {
        NodeType* node = AllocateNode();
        node->storage.Construct(std::forward<Args>(args)...);

        node->previous = lastNode;
        node->next = nullptr;

        if (node->previous)
            node->previous->next = node;
        else
            firstNode = node;

        lastNode = node;

        ++numAllocated;
    }

    template<class T>
    bool BoundedList<T>::operator==(const BoundedList& other) const
    {
        return size() == other.size()
            && std::equal(begin(), end(), other.begin());
    }

    template<class T>
    bool BoundedList<T>::operator!=(const BoundedList& other) const
    {
        return !(*this == other);
    }

    template<class T>
    bool BoundedList<T>::operator<(const BoundedList<T>& other) const
    {
        return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }

    template<class T>
    bool BoundedList<T>::operator<=(const BoundedList<T>& other) const
    {
        return !(other < *this);
    }

    template<class T>
    bool BoundedList<T>::operator>(const BoundedList<T>& other) const
    {
        return other < *this;
    }

    template<class T>
    bool BoundedList<T>::operator>=(const BoundedList<T>& other) const
    {
        return !(*this < other);
    }

    template<class T>
    typename BoundedList<T>::NodeType* BoundedList<T>::AllocateNode()
    {
        NodeType* result;

        if (freeList)
        {
            result = freeList;
            freeList = freeList->next;
            return result;
        }

        really_assert(numInitialized != storage.size());

        result = &*storage[numInitialized];
        ++numInitialized;
        return result;
    }

    template<class T>
    void swap(BoundedList<T>& x, BoundedList<T>& y)
    {
        x.swap(y);
    }

    namespace detail
    {

        template<class T>
        BoundedListIterator<T>::BoundedListIterator()
            : mNode(0)
        {}

        template<class T>
        BoundedListIterator<T>::BoundedListIterator(NodeType* node)
            : mNode(node)
        {}

        template<class T>
        template<class T2>
        BoundedListIterator<T>::BoundedListIterator(const BoundedListIterator<T2>& other)
            : mNode(other.mNode)
        {}

        template<class T>
        template<class T2>
        BoundedListIterator<T>& BoundedListIterator<T>::operator=(const BoundedListIterator<T2>& other)
        {
            mNode = other.mNode;

            return *this;
        }

        template<class T>
        T& BoundedListIterator<T>::operator*() const
        {
            return *mNode->storage;
        }

        template<class T>
        T* BoundedListIterator<T>::operator->() const
        {
            return &*mNode->storage;
        }

        template<class T>
        typename BoundedListIterator<T>::NodeType& BoundedListIterator<T>::node()
        {
            really_assert(mNode != 0);
            return *mNode;
        }

        template<class T>
        BoundedListIterator<T>& BoundedListIterator<T>::operator++()
        {
            mNode = mNode->next;
            return *this;
        }

        template<class T>
        BoundedListIterator<T> BoundedListIterator<T>::operator++(int)
        {
            BoundedListIterator copy(*this);
            mNode = mNode->next;
            return copy;
        }

        template<class T>
        BoundedListIterator<T>& BoundedListIterator<T>::operator--()
        {
            mNode = mNode->previous;
            return *this;
        }

        template<class T>
        BoundedListIterator<T> BoundedListIterator<T>::operator--(int)
        {
            BoundedListIterator copy(*this);
            mNode = mNode->previous;
            return copy;
        }

        template<class T>
        template<class T2>
        bool BoundedListIterator<T>::operator==(const BoundedListIterator<T2>& other) const
        {
            return mNode == other.mNode;
        }

        template<class T>
        template<class T2>
        bool BoundedListIterator<T>::operator!=(const BoundedListIterator<T2>& other) const
        {
            return !(*this == other);
        }
    }
}

#endif
