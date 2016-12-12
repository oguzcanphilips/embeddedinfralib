#ifndef INFRA_SHARED_PTR_HPP
#define INFRA_SHARED_PTR_HPP

#include <cstdint>
#include <type_traits>

namespace infra
{
    class SharedObjectAllocatorBase;

    template<class T>
    class WeakPtr;

    namespace detail
    {
        class SharedPtrControl
        {
        public:
            SharedPtrControl(const void* object, SharedObjectAllocatorBase* allocator);

            void IncreaseSharedCount();
            void DecreaseSharedCount();
            void IncreaseWeakCount();
            void DecreaseWeakCount();

            bool Expired() const;

        private:
            uint16_t sharedPtrCount = 0;
            uint16_t weakPtrCount = 0;
            const void* object = nullptr;
            SharedObjectAllocatorBase* allocator = nullptr;
        };
    }

    template<class T>
    class SharedPtr
    {
    public:
        SharedPtr() = default;
        SharedPtr(std::nullptr_t);                                                                                      //TICS !INT#001
        SharedPtr(detail::SharedPtrControl* control, T* object);
        SharedPtr(const SharedPtr& other);
        SharedPtr(SharedPtr&& other);
        SharedPtr(const WeakPtr<T>& other);                                                                             //TICS !INT#001
        template<class U>                                                                                               //TICS !INT#001
            SharedPtr(const SharedPtr<U>& other);
        template<class U>                                                                                               //TICS !INT#001
            SharedPtr(SharedPtr<U>&& other);
        SharedPtr& operator=(const SharedPtr& other);
        SharedPtr& operator=(SharedPtr&& other);
        SharedPtr& operator=(const WeakPtr<T>& other);
        SharedPtr& operator=(std::nullptr_t);
        ~SharedPtr();

        explicit operator bool() const;
        T* operator->() const;
        typename std::add_lvalue_reference<T>::type operator*() const;

        bool operator==(const SharedPtr& other) const;
        bool operator!=(const SharedPtr& other) const;
        bool operator==(std::nullptr_t) const;
        bool operator!=(std::nullptr_t) const;
        friend bool operator==(std::nullptr_t, const SharedPtr& ptr) { return ptr == nullptr; }
        friend bool operator!=(std::nullptr_t, const SharedPtr& ptr) { return ptr != nullptr; }

    private:
        void Reset(detail::SharedPtrControl* newControl, T* object);

    private:
        template<class U>
            friend class SharedPtr;

        template<class U, class V>
            friend SharedPtr<U> StaticPointerCast(const SharedPtr<V>& sharedPtr);
        template<class U, class V>
            friend SharedPtr<U> StaticPointerCast(SharedPtr<V>&& sharedPtr);
        template<class U>
            friend SharedPtr<typename std::remove_const<U>::type> ConstCast(const SharedPtr<U>& sharedPtr);
        template<class U>
            friend SharedPtr<typename std::remove_const<U>::type> ConstCast(SharedPtr<U>&& sharedPtr);

        template<class U>
            friend class WeakPtr;

    private:
        detail::SharedPtrControl* control = nullptr;
        T* object = nullptr;
    };

    template<class T>
    class WeakPtr
    {
    public:
        WeakPtr() = default;
        WeakPtr(const WeakPtr<T>& other);
        WeakPtr(WeakPtr<T>&& other);
        WeakPtr(const SharedPtr<T>& sharedPtr);                                                                         //TICS !INT#001
        WeakPtr& operator=(const WeakPtr<T>& other);
        WeakPtr& operator=(WeakPtr<T>&& other);
        WeakPtr& operator=(const SharedPtr<T>& sharedPtr);
        ~WeakPtr();

        SharedPtr<T> lock() const;

        bool operator==(const WeakPtr& other) const;
        bool operator!=(const WeakPtr& other) const;
        bool operator==(std::nullptr_t) const;
        bool operator!=(std::nullptr_t) const;
        friend bool operator==(std::nullptr_t, const WeakPtr& ptr) { return ptr == nullptr; }
        friend bool operator!=(std::nullptr_t, const WeakPtr& ptr) { return ptr != nullptr; }
        bool operator==(const SharedPtr<T>& other) const;
        bool operator!=(const SharedPtr<T>& other) const;
        friend bool operator==(const SharedPtr<T>& left, const WeakPtr& right) { return right == left; }
        friend bool operator!=(const SharedPtr<T>& left, const WeakPtr& right) { return right != left; }

    private:
        void Reset(detail::SharedPtrControl* newControl, T* newObject);

        template<class U>
            friend class SharedPtr;

    private:
        detail::SharedPtrControl* control = nullptr;
        T* object = nullptr;
    };

    ////    Implementation    ////

    template<class T>
    SharedPtr<T>::SharedPtr(std::nullptr_t)
    {}

    template<class T>
    SharedPtr<T>::SharedPtr(detail::SharedPtrControl* control, T* object)
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
    SharedPtr<T>::SharedPtr(const WeakPtr<T>& other)
    {
        if (other.control && !other.control->Expired())
            Reset(other.control, other.object);
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
    SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other)
    {
        if (this != &other)
            Reset(other.control, other.object);

        return *this;
    }

    template<class T>
    SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other)
    {
        Reset(other.control, other.object);
        other.Reset(nullptr, nullptr);

        return *this;
    }

    template<class T>
    SharedPtr<T>& SharedPtr<T>::operator=(const WeakPtr<T>& other)
    {
        Reset(other.control, other.object);

        return *this;
    }
    
    template<class T>
    SharedPtr<T>& SharedPtr<T>::operator=(std::nullptr_t)
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
    typename std::add_lvalue_reference<T>::type SharedPtr<T>::operator*() const
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
    void SharedPtr<T>::Reset(detail::SharedPtrControl* newControl, T* newObject)
    {
        if (control)
            control->DecreaseSharedCount();

        control = newControl;
        object = newObject;

        if (control)
            control->IncreaseSharedCount();
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

    template<class T>
    WeakPtr<T>::WeakPtr(const WeakPtr& other)
    {
        Reset(other.control, other.object);
    }

    template<class T>
    WeakPtr<T>::WeakPtr(WeakPtr&& other)
    {
        Reset(other.control, other.object);
        other.Reset(nullptr, nullptr);
    }

    template<class T>
    WeakPtr<T>::WeakPtr(const SharedPtr<T>& sharedPtr)
    {
        Reset(sharedPtr.control, sharedPtr.object);
    }

    template<class T>
    WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& other)
    {
        Reset(other.control, other.object);

        return *this;
    }

    template<class T>
    WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other)
    {
        Reset(other.control, other.object);
        other.Reset(nullptr, nullptr);

        return *this;
    }

    template<class T>
    WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& sharedPtr)
    {
        Reset(sharedPtr.control, sharedPtr.object);

        return *this;
    }

    template<class T>
    WeakPtr<T>::~WeakPtr()
    {
        Reset(nullptr, nullptr);
    }

    template<class T>
    SharedPtr<T> WeakPtr<T>::lock() const
    {
        return SharedPtr<T>(*this);
    }

    template<class T>
    bool WeakPtr<T>::operator==(const WeakPtr& other) const
    {
        return object == other.object;
    }

    template<class T>
    bool WeakPtr<T>::operator!=(const WeakPtr& other) const
    {
        return !(*this == other);
    }

    template<class T>
    bool WeakPtr<T>::operator==(std::nullptr_t) const
    {
        return object == nullptr;
    }

    template<class T>
    bool WeakPtr<T>::operator!=(std::nullptr_t) const
    {
        return !(*this == nullptr);
    }

    template<class T>
    bool WeakPtr<T>::operator==(const SharedPtr<T>& other) const
    {
        return object == other.object;
    }

    template<class T>
    bool WeakPtr<T>::operator!=(const SharedPtr<T>& other) const
    {
        return !(*this == other);
    }

    template<class T>
    void WeakPtr<T>::Reset(detail::SharedPtrControl* newControl,T* newObject)
    {
        if (control)
            control->DecreaseWeakCount();

        control = newControl;
        object = newObject;

        if (control)
            control->IncreaseWeakCount();
    }
}

#endif
