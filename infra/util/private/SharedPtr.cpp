#include "infra/util/public/SharedPtr.hpp"
#include "infra/util/public/SharedObjectAllocator.hpp"
#include <cassert>

namespace infra
{
    namespace detail
    {
        SharedPtrControl::SharedPtrControl(const void* object, SharedObjectAllocatorBase* allocator)
            : object(object)
            , allocator(allocator)
        {}

        void SharedPtrControl::IncreaseSharedCount()
        {
            ++sharedPtrCount;
            IncreaseWeakCount();
        }

        void SharedPtrControl::DecreaseSharedCount()
        {
            assert(sharedPtrCount != 0);
                
            --sharedPtrCount;
            if (sharedPtrCount == 0)
                allocator->Destruct(object);

            DecreaseWeakCount();
        }

        void SharedPtrControl::IncreaseWeakCount()
        {
            ++weakPtrCount;
        }

        void SharedPtrControl::DecreaseWeakCount()
        {
            assert(weakPtrCount != 0);

            --weakPtrCount;
            if (weakPtrCount == 0)
                allocator->Deallocate(this);
        }

        bool SharedPtrControl::Expired() const
        {
            return sharedPtrCount == 0;
        }
    }
}
