#ifndef INFRA_ALLOCATOR_HPP
#define INFRA_ALLOCATOR_HPP

#include <memory>

namespace infra
{
    class AllocatorBase
    {
    public:
        virtual void Deallocate(void* object) = 0;
    };

    class Deallocator
    {
    public:
        Deallocator() = default;
        Deallocator(AllocatorBase& allocator);

        void operator()(void* object);

    private:
        AllocatorBase* allocator = nullptr;
    };

    template<class T>
        using UniquePtr = std::unique_ptr<T, Deallocator>;

    template<class T>
        UniquePtr<T> MakeUnique(T* object, AllocatorBase& allocator);

    template<class T, class... ConstructionArgs>
    class Allocator
        : public AllocatorBase
    {
    public:
        virtual UniquePtr<T> Allocate(ConstructionArgs... args) = 0;

    protected:
        ~Allocator() = default;
    };

    ////    Implementation    ////

    inline Deallocator::Deallocator(AllocatorBase& allocator)
        : allocator(&allocator)
    {}

    inline void Deallocator::operator()(void* object)
    {
        allocator->Deallocate(object);
    }

    template<class T>
    UniquePtr<T> MakeUnique(T* object, AllocatorBase& allocator)
    {
        return infra::UniquePtr<T>(object, Deallocator(allocator));
    }
}

#endif
