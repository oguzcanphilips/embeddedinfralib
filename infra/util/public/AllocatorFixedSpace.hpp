#ifndef INFRA_ALLOCATOR_FIXED_SPACE_HPP
#define INFRA_ALLOCATOR_FIXED_SPACE_HPP

#include "infra/util/public/Allocator.hpp"
#include "infra/util/public/BoundedVector.hpp"
#include "infra/util/public/ReallyAssert.hpp"
#include "infra/util/public/StaticStorage.hpp"

namespace infra
{
    template<class T, std::size_t NumberOfElements, class... ConstructionArgs>
    class AllocatorFixedSpace
        : public Allocator<T, ConstructionArgs...>
    {
        static_assert(sizeof(T) == sizeof(StaticStorage<T>), "sizeof(StaticStorage) must be equal to sizeof(T) else reinterpret_cast will fail");
    public:
        virtual UniquePtr<T> Allocate(ConstructionArgs... args) override;
        virtual void Deallocate(void* object) override;
        bool Empty() const;

    private:
        struct Node
            : public StaticStorage<T>
        {
            Node* next;
        };

        Node* AllocateNode();

    private:
        typename infra::BoundedVector<Node>::template WithMaxSize<NumberOfElements> elements;
        Node* freeList = nullptr;
    };

    ////    Implementation    ////

    template<class T, std::size_t NumberOfElements, class... ConstructionArgs>
    UniquePtr<T> AllocatorFixedSpace<T, NumberOfElements, ConstructionArgs...>::Allocate(ConstructionArgs... args)
    {
        Node* node = AllocateNode();
        node->Construct(std::forward<ConstructionArgs>(args)...);

        return MakeUnique<T>(&**node, *this);
    }

    template<class T, std::size_t NumberOfElements, class... ConstructionArgs>
    void AllocatorFixedSpace<T, NumberOfElements, ConstructionArgs...>::Deallocate(void* object)
    {
        Node* node = static_cast<Node*>(reinterpret_cast<StaticStorage<T>*>(static_cast<T*>(object)));
        node->Destruct();

        node->next = freeList;
        freeList = node;
    }

    template<class T, std::size_t NumberOfElements, class... ConstructionArgs>
    bool AllocatorFixedSpace<T, NumberOfElements, ConstructionArgs...>::Empty() const
    {
        return freeList == nullptr && elements.full();
    }

    template<class T, std::size_t NumberOfElements, class... ConstructionArgs>
    typename AllocatorFixedSpace<T, NumberOfElements, ConstructionArgs...>::Node* AllocatorFixedSpace<T, NumberOfElements, ConstructionArgs...>::AllocateNode()
    {
        if (freeList)
        {
            Node* result = freeList;
            freeList = freeList->next;
            return result;
        }
        else
        {
            really_assert(!elements.full());
            elements.emplace_back();
            return &elements.back();
        }
    }
}

#endif
