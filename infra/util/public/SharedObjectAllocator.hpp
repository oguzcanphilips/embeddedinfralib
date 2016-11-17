#ifndef INFRA_SHARED_OBJECT_ALLOCATOR_HPP
#define INFRA_SHARED_OBJECT_ALLOCATOR_HPP

#include "infra/util/public/SharedPtr.hpp"

namespace infra
{
    class SharedObjectAllocatorBase
    {
    protected:
        SharedObjectAllocatorBase() = default;
        SharedObjectAllocatorBase(const SharedObjectAllocatorBase& other) = delete;
        SharedObjectAllocatorBase& operator=(const SharedObjectAllocatorBase& other) = delete;
        ~SharedObjectAllocatorBase() = default;

    public:
        virtual void Destruct(const void* object) = 0;
        virtual void Deallocate(void* control) = 0;
    };

    template<class T, class ConstructionArgs>
    class SharedObjectAllocator;

    template<class T, class... ConstructionArgs>
    class SharedObjectAllocator<T, void(ConstructionArgs...)>
        : public SharedObjectAllocatorBase
    {
    public:
        virtual SharedPtr<T> Allocate(ConstructionArgs... args) = 0;
    };
}

#endif
