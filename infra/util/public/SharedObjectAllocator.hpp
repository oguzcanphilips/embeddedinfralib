#ifndef INFRA_SHARED_OBJECT_ALLOCATOR_HPP
#define INFRA_SHARED_OBJECT_ALLOCATOR_HPP

#include "infra/util/public/SharedPtr.hpp"

namespace infra
{
    template<class T, class ConstructionArgs>
    class SharedObjectAllocator;

    template<class T, class... ConstructionArgs>
    class SharedObjectAllocator<T, void(ConstructionArgs...)>
        : public SharedObjectAllocatorBase
    {
    protected:
        ~SharedObjectAllocator() = default;

    public:
        virtual SharedPtr<T> Allocate(ConstructionArgs... args) = 0;
    };
}

#endif
