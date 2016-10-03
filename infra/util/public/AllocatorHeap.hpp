#ifndef INFRA_ALLOCATOR_HEAP_HPP
#define INFRA_ALLOCATOR_HEAP_HPP

#include "infra/util/public/Allocator.hpp"

namespace infra
{
    template<class T, class... ConstructionArgs>
    class AllocatorHeap
        : public Allocator<T, ConstructionArgs...>
    {
    public:
        virtual UniquePtr<T> Allocate(ConstructionArgs... args) override;
        virtual void Deallocate(void* object) override;
    };

    ////    Implementation    ////

    template<class T, class... ConstructionArgs>
    UniquePtr<T> AllocatorHeap<T, ConstructionArgs...>::Allocate(ConstructionArgs... args)
    {
        T* object = new (std::nothrow) T(args...);
        if (object)
            return MakeUnique(object, *this);
        else
            return UniquePtr<T>();
    }

    template<class T, class... ConstructionArgs>
    void AllocatorHeap<T, ConstructionArgs...>::Deallocate(void* object)
    {
        delete static_cast<T*>(object);
    }
}

#endif
