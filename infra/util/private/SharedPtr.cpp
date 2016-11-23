#include "infra/util/public/SharedPtr.hpp"
#include "infra/util/public/SharedObjectAllocator.hpp"

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
