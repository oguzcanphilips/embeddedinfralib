#ifndef INFRA_SHARED_PTR_HPP
#define INFRA_SHARED_PTR_HPP

#include <type_traits>

namespace infra
{
    class SharedObjectAllocatorBase;

    class SharedPtrControl
    {
    public:
        uint16_t sharedPtrCount = 0;
        uint16_t weakPtrCount = 0;
        SharedObjectAllocatorBase* allocator = nullptr;
    };

    template<class T>
    class SharedPtr
    {
    public:
        SharedPtr() = default;
        explicit SharedPtr(SharedPtrControl* control, T* object);
        SharedPtr(const SharedPtr& other);
        SharedPtr(SharedPtr&& other);
        template<class U>
            SharedPtr(const SharedPtr<U>& other);
        template<class U>
            SharedPtr(SharedPtr<U>&& other);
        SharedPtr operator=(const SharedPtr& other);
        SharedPtr operator=(SharedPtr&& other);
        SharedPtr operator=(std::nullptr_t);
        ~SharedPtr();

        explicit operator bool() const;
        T* operator->() const;
        T& operator*() const;

        bool operator==(const SharedPtr& other) const;
        bool operator!=(const SharedPtr& other) const;
        bool operator==(std::nullptr_t) const;
        bool operator!=(std::nullptr_t) const;
        friend bool operator==(std::nullptr_t, const SharedPtr& ptr) { return ptr == nullptr; }
        friend bool operator!=(std::nullptr_t, const SharedPtr& ptr) { return ptr != nullptr; }

    private:
        void Reset(SharedPtrControl* newControl, T* object);
        void IncreaseSharedCount();

    private:
        template<class U>
            friend class SharedPtr;

        template<class U, class T>
            friend SharedPtr<U> StaticPointerCast(const SharedPtr<T>& sharedPtr);
        template<class U, class T>
            friend SharedPtr<U> StaticPointerCast(SharedPtr<T>&& sharedPtr);
        template<class T>
            friend SharedPtr<typename std::remove_const<T>::type> ConstCast(const SharedPtr<T>& sharedPtr);
        template<class T>
            friend SharedPtr<typename std::remove_const<T>::type> ConstCast(SharedPtr<T>&& sharedPtr);

        SharedPtrControl* control = nullptr;
        T* object = nullptr;
    };

    ////    Implementation    ////

    template<class T>
    SharedPtr<T>::SharedPtr(SharedPtrControl* control, T* object)
    {
        Reset(control, object);
    }

    template<class T>
    SharedPtr<T>::SharedPtr(const SharedPtr& other)
    {
        Reset(other.control, other.object);
    }

    template<class T>
    SharedPtr<T>::SharedPtr(SharedPtr&& other)
    {
        Reset(other.control, other.object);
        other.Reset(nullptr, nullptr);
    }

    template<class T>
    template<class U>
    SharedPtr<T>::SharedPtr(const SharedPtr<U>& other)
    {
        Reset(other.control, other.object);
    }

    template<class T>
    template<class U>
    SharedPtr<T>::SharedPtr(SharedPtr<U>&& other)
    {
        Reset(other.control, other.object);
        other.Reset(nullptr, nullptr);
    }

    template<class T>
    SharedPtr<T> SharedPtr<T>::operator=(const SharedPtr& other)
    {
        if (this != &other)
            Reset(other.control, other.object);

        return *this;
    }

    template<class T>
    SharedPtr<T> SharedPtr<T>::operator=(SharedPtr&& other)
    {
        Reset(other.control, other.object);
        other.Reset(nullptr, nullptr);

        return *this;
    }

    template<class T>
    SharedPtr<T> SharedPtr<T>::operator=(std::nullptr_t)
    {
        Reset(nullptr, nullptr);
        return *this;
    }

    template<class T>
    SharedPtr<T>::~SharedPtr()
    {
        Reset(nullptr, nullptr);
    }

    template<class T>
    SharedPtr<T>::operator bool() const
    {
        return control != nullptr;
    }

    template<class T>
    T* SharedPtr<T>::operator->() const
    {
        assert(object != nullptr);
        return object;
    }

    template<class T>
    T& SharedPtr<T>::operator*() const
    {
        assert(object != nullptr);
        return *object;
    }

    template<class T>
    bool SharedPtr<T>::operator==(const SharedPtr& other) const
    {
        return object == other.object;
    }

    template<class T>
    bool SharedPtr<T>::operator!=(const SharedPtr& other) const
    {
        return !(*this == other);
    }

    template<class T>
    bool SharedPtr<T>::operator==(std::nullptr_t) const
    {
        return object == nullptr;
    }

    template<class T>
    bool SharedPtr<T>::operator!=(std::nullptr_t) const
    {
        return !(*this == nullptr);
    }

    template<class T>
    void SharedPtr<T>::Reset(SharedPtrControl* newControl, T* newObject)
    {
        if (control)
        {
            --control->sharedPtrCount;
            --control->weakPtrCount;

            if (control->sharedPtrCount == 0)
            {
                control->allocator->Destruct(object);
                control->allocator->Deallocate(control);
            }
        }

        control = newControl;
        object = newObject;

        if (control)
            IncreaseSharedCount();
    }

    template<class T>
    void SharedPtr<T>::IncreaseSharedCount()
    {
        ++control->sharedPtrCount;
        ++control->weakPtrCount;
    }

    template<class U, class T>
    SharedPtr<U> StaticPointerCast(const SharedPtr<T>& sharedPtr)
    {
        return SharedPtr<U>(sharedPtr.control, static_cast<U*>(sharedPtr.object));
    }

    template<class U, class T>
    SharedPtr<U> StaticPointerCast(SharedPtr<T>&& sharedPtr)
    {
        SharedPtr<U> result(sharedPtr.control, static_cast<U*>(sharedPtr.object));
        sharedPtr.Reset(nullptr, nullptr);
        return result;
    }

    template<class T>
    SharedPtr<typename std::remove_const<T>::type> ConstCast(const SharedPtr<T>& sharedPtr)
    {
        return SharedPtr<typename std::remove_const<T>::type>(sharedPtr.control, const_cast<typename std::remove_const<T>::type*>(sharedPtr.object));
    }

    template<class T>
    SharedPtr<typename std::remove_const<T>::type> ConstCast(SharedPtr<T>&& sharedPtr)
    {
        SharedPtr<typename std::remove_const<T>::type> result(sharedPtr.control, const_cast<typename std::remove_const<T>::type*>(sharedPtr.object));
        sharedPtr.Reset(nullptr, nullptr);
        return result;
    }
}

#endif
