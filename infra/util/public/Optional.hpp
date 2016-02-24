#ifndef INFRA_OPTIONAL_HPP
#define INFRA_OPTIONAL_HPP

//  Optional<T> is a class that either holds an object of type T, or it does not. With
//  this class, you can delay construction of an object, or hasten destruction. The storage
//  needed is allocated inside of the optional. Basically, sizeof(Optional<T>) == sizeof(T) + 1.
//  It uses pointer-like operations for access. Examples:
//  
//  Optional<MyClass> opt(inPlace, 2, 3, 4); // 2, 3, 4 is forwarded to the constructor of MyClass
//
//  if (opt)                        // Test whether opt contains a value
//      opt->MyFunc();              // Use * and -> to access the value
//  opt = nullptr;                  // Reset the optional by assigning nullptr
//  if (!opt)
//      opt.Emplace(2, 3, 4);       // Late construction by explicitly calling Emplace
//  opt = MyClass(2, 3, 4);         // Use the copy constructor for construction

#include "infra/util/public/Compare.hpp"
#include "infra/util/public/StaticStorage.hpp"
#include <memory>
#include <type_traits>

namespace infra
{

    struct None {};
    const None none;

    struct InPlace {};
    const InPlace inPlace;

    template<class T>
    class Optional
        : public EqualityComparable<Optional<T>>
        , public EqualityComparableHeterogeneous<Optional<T>, T>
        , public EqualityComparableHeterogeneous<Optional<T>, None>
    {
    public:
        Optional();

        Optional(const Optional& other);
        Optional(Optional&& other);
        Optional(None);
        template<class... Args>
            Optional(InPlace, Args&&... args);

        ~Optional();

        Optional& operator=(const Optional& other);
        Optional& operator=(Optional&& other);
        Optional& operator=(const None&);
        Optional& operator=(const T& value);
        Optional& operator=(T&& value);

        template<class... Args>
            void Emplace(Args&&... args);
        template<class U, class... Args>
            void Emplace(std::initializer_list<U> list, Args&&... args);

        const T& operator*() const;
        T& operator*();
        const T* operator->() const;
        T* operator->();

        explicit operator bool() const;

        bool operator!() const;
        bool operator==(const Optional& other) const;
        bool operator==(const None& other) const;
        bool operator==(const T& other) const;

        template<class U>
        T ValueOr(U&& value) const;

    private:
        void Reset();

    private:
        bool mInitialized;
        StaticStorage<T> mData;
    };

    template<class T> 
        Optional<typename std::decay<T>::type> MakeOptional(T&& value);

    template<class T, std::size_t ExtraSize>
    class OptionalForPolymorphicObjects
        : public EqualityComparable<OptionalForPolymorphicObjects<T, ExtraSize>>
        , public EqualityComparableHeterogeneous<OptionalForPolymorphicObjects<T, ExtraSize>, T>
        , public EqualityComparableHeterogeneous<OptionalForPolymorphicObjects<T, ExtraSize>, None>
    {
    public:
        OptionalForPolymorphicObjects();

        OptionalForPolymorphicObjects(const OptionalForPolymorphicObjects& other) = delete;
        template<class Derived, std::size_t OtherExtraSize>
            OptionalForPolymorphicObjects(const OptionalForPolymorphicObjects<Derived, OtherExtraSize>& other, typename std::enable_if<std::is_base_of<T, Derived>::value>::type* = 0);
        OptionalForPolymorphicObjects(None);
        template<class... Args>
            OptionalForPolymorphicObjects(InPlace, Args&&... args);

        ~OptionalForPolymorphicObjects();

        OptionalForPolymorphicObjects& operator=(const OptionalForPolymorphicObjects& other) = delete;
        template<class Derived, std::size_t OtherExtraSize>
            typename std::enable_if<std::is_base_of<T, Derived>::value, OptionalForPolymorphicObjects&>::type
            operator=(const OptionalForPolymorphicObjects<Derived, OtherExtraSize>& other);
        OptionalForPolymorphicObjects& operator=(const None&);
        OptionalForPolymorphicObjects& operator=(const T& value);
        OptionalForPolymorphicObjects& operator=(T&& value);

        template<class Derived, class... Args>
            void Emplace(Args&&... args);

        const T& operator*() const;
        T& operator*();
        const T* operator->() const;
        T* operator->();

        explicit operator bool() const;

        bool operator!() const;
        bool operator==(const OptionalForPolymorphicObjects& other) const;
        bool operator==(const None& other) const;
        bool operator==(const T& other) const;

    private:
        void Reset();

    private:
        bool mInitialized;
        StaticStorageForPolymorphicObjects<T, ExtraSize> mData;
    };

    template<class T>
        OptionalForPolymorphicObjects<typename std::decay<T>::type, 0> MakeOptionalForPolymorphicObjects(T&& value);
        
    ////    Implementation    ////

    template<class T>
    Optional<T>::Optional()
        : mInitialized(false)
    {}

    template<class T>
    Optional<T>::Optional(const Optional& other)
        : mInitialized(false)
    {
        if (other)
            Emplace(*other);
    }

    template<class T>
    Optional<T>::Optional(Optional&& other)
        : mInitialized(false)
    {
        if (other)
        {
            Emplace(std::move(*other));
            other.Reset();
        }
    }

    template<class T>
    Optional<T>::Optional(None)
        : mInitialized(false)
    {}

    template<class T>
    template<class... Args>
    Optional<T>::Optional(InPlace, Args&&... args)
        : mInitialized(false)
    {
        Emplace(std::forward<Args>(args)...);
    }

    template<class T>
    Optional<T>::~Optional()
    {
        Reset();
    }

    template<class T>
    Optional<T>& Optional<T>::operator=(const Optional& other)
    {
        if (this != &other)
        {
            Reset();
            if (other)
                Emplace(*other);
        }

        return *this;
    }

    template<class T>
    Optional<T>& Optional<T>::operator=(Optional&& other)
    {
        if (this != &other)
        {
            Reset();
            if (other)
            {
                Emplace(std::move(*other));
                other.Reset();
            }
        }

        return *this;
    }

    template<class T>
    Optional<T>& Optional<T>::operator=(const None&)
    {
        Reset();
        return *this;
    }

    template<class T>
    Optional<T>& Optional<T>::operator=(const T& value)
    {
        Reset();
        Emplace(value);
        return *this;
    }

    template<class T>
    Optional<T>& Optional<T>::operator=(T&& value)
    {
        Reset();
        Emplace(std::forward<T>(value));
        return *this;
    }

    template<class T>
    const T& Optional<T>::operator*() const
    {
        return *mData;
    }

    template<class T>
    T& Optional<T>::operator*()
    {
        return *mData;
    }

    template<class T>
    const T* Optional<T>::operator->() const
    {
        return &*mData;
    }

    template<class T>
    T* Optional<T>::operator->()
    {
        return &*mData;
    }

    template<class T>
    Optional<T>::operator bool() const
    {
        return mInitialized;
    }

    template<class T>
    bool Optional<T>::operator!() const
    {
        return !mInitialized;
    }

    template<class T>
    bool Optional<T>::operator==(const Optional& other) const
    {
        if (mInitialized && other.mInitialized)
            return **this == *other;
        else
            return mInitialized == other.mInitialized;
    }

    template<class T>
    bool Optional<T>::operator==(const None& other) const
    {
        return !mInitialized;
    }

    template<class T>
    bool Optional<T>::operator==(const T& other) const
    {
        return mInitialized && **this == other;
    }

    template<class T>
    template<class U>
    T Optional<T>::ValueOr(U&& value) const
    {
        if (mInitialized)
            return **this;
        else
            return std::move(value);
    }

    template<class T>
    template<class... Args>
    void Optional<T>::Emplace(Args&&... args)
    {
        Reset();
        mData.Construct(std::forward<Args>(args)...);
        mInitialized = true;
    }

    template<class T>
    template<class U, class... Args>
    void Optional<T>::Emplace(std::initializer_list<U> list, Args&&... args)
    {
        Reset();
        mData.Construct(list, std::forward<Args>(args)...);
        mInitialized = true;
    }

    template<class T>
    void Optional<T>::Reset()
    {
        if (mInitialized)
        {
            mData.Destruct();
            mInitialized = false;
        }
    }

    template<class T> 
    Optional<typename std::decay<T>::type> MakeOptional(T&& value)
    {
        return Optional<typename std::decay<T>::type>(inPlace, std::forward<T>(value));
    }

    template<class T, std::size_t ExtraSize>
    OptionalForPolymorphicObjects<T, ExtraSize>::OptionalForPolymorphicObjects()
        : mInitialized(false)
    {}

    template<class T, std::size_t ExtraSize>
    template<class Derived, std::size_t OtherExtraSize>
    OptionalForPolymorphicObjects<T, ExtraSize>::OptionalForPolymorphicObjects(const OptionalForPolymorphicObjects<Derived, OtherExtraSize>& other
            , typename std::enable_if<std::is_base_of<T, Derived>::value>::type*)
        : mInitialized(false)
    {
        if (other)
            Emplace<Derived>(*other);
    }

    template<class T, std::size_t ExtraSize>
    OptionalForPolymorphicObjects<T, ExtraSize>::OptionalForPolymorphicObjects(None)
        : mInitialized(false)
    {}

    template<class T, std::size_t ExtraSize>
    template<class... Args>
    OptionalForPolymorphicObjects<T, ExtraSize>::OptionalForPolymorphicObjects(InPlace, Args&&... args)
        : mInitialized(false)
    {
        Emplace<T>(std::forward<Args>(args)...);
    }

    template<class T, std::size_t ExtraSize>
    OptionalForPolymorphicObjects<T, ExtraSize>::~OptionalForPolymorphicObjects()
    {
        Reset();
    }

    template<class T, std::size_t ExtraSize>
    template<class Derived, std::size_t OtherExtraSize>
    typename std::enable_if<std::is_base_of<T, Derived>::value, OptionalForPolymorphicObjects<T, ExtraSize>&>::type
        OptionalForPolymorphicObjects<T, ExtraSize>::operator=(const OptionalForPolymorphicObjects<Derived, OtherExtraSize>& other)
    {
        Reset();
        if (other)
            Emplace<Derived>(*other);
        return *this;
    }

    template<class T, std::size_t ExtraSize>
    OptionalForPolymorphicObjects<T, ExtraSize>& OptionalForPolymorphicObjects<T, ExtraSize>::operator=(const None&)
    {
        Reset();
        return *this;
    }

    template<class T, std::size_t ExtraSize>
    OptionalForPolymorphicObjects<T, ExtraSize>& OptionalForPolymorphicObjects<T, ExtraSize>::operator=(const T& value)
    {
        Reset();
        Emplace<T>(value);
        return *this;
    }

    template<class T, std::size_t ExtraSize>
    OptionalForPolymorphicObjects<T, ExtraSize>& OptionalForPolymorphicObjects<T, ExtraSize>::operator=(T&& value)
    {
        Reset();
        Emplace<T>(std::forward<T>(value));
        return *this;
    }

    template<class T, std::size_t ExtraSize>
    const T& OptionalForPolymorphicObjects<T, ExtraSize>::operator*() const
    {
        return *mData;
    }

    template<class T, std::size_t ExtraSize>
    T& OptionalForPolymorphicObjects<T, ExtraSize>::operator*()
    {
        return *mData;
    }

    template<class T, std::size_t ExtraSize>
    const T* OptionalForPolymorphicObjects<T, ExtraSize>::operator->() const
    {
        return &*mData;
    }

    template<class T, std::size_t ExtraSize>
    T* OptionalForPolymorphicObjects<T, ExtraSize>::operator->()
    {
        return &*mData;
    }

    template<class T, std::size_t ExtraSize>
    OptionalForPolymorphicObjects<T, ExtraSize>::operator bool() const
    {
        return mInitialized;
    }

    template<class T, std::size_t ExtraSize>
    bool OptionalForPolymorphicObjects<T, ExtraSize>::operator!() const
    {
        return !mInitialized;
    }

    template<class T, std::size_t ExtraSize>
    bool OptionalForPolymorphicObjects<T, ExtraSize>::operator==(const OptionalForPolymorphicObjects& other) const
    {
        if (mInitialized && other.mInitialized)
            return **this == *other;
        else
            return mInitialized == other.mInitialized;
    }

    template<class T, std::size_t ExtraSize>
    bool OptionalForPolymorphicObjects<T, ExtraSize>::operator==(const None& other) const
    {
        return !mInitialized;
    }

    template<class T, std::size_t ExtraSize>
    bool OptionalForPolymorphicObjects<T, ExtraSize>::operator==(const T& other) const
    {
        return mInitialized && **this == other;
    }

    template<class T, std::size_t ExtraSize>
    template<class Derived, class... Args>
    void OptionalForPolymorphicObjects<T, ExtraSize>::Emplace(Args&&... args)
    {
        Reset();
        mData.template Construct<Derived>(std::forward<Args>(args)...);
        mInitialized = true;
    }

    template<class T, std::size_t ExtraSize>
    void OptionalForPolymorphicObjects<T, ExtraSize>::Reset()
    {
        if (mInitialized)
        {
            mData.Destruct();
            mInitialized = false;
        }
    }

    template<class T>
    OptionalForPolymorphicObjects<typename std::decay<T>::type, 0> MakeOptionalForPolymorphicObjects(T&& value)
    {
        return OptionalForPolymorphicObjects<typename std::decay<T>::type, 0>(inPlace, std::forward<T>(value));
    }
}

#endif
