#ifndef INFRA_STATIC_STORAGE_HPP
#define INFRA_STATIC_STORAGE_HPP

#include "infra/util/public/VariadicTemplates.hpp"
#include <cstdint>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace infra
{
    // StaticStorage provides storage for an object, so that the object can be constructed and destroyed without
    // the need for allocating or releasing memory.
    //
    // When StaticStorage is a global object, it is allocated during constant initialization, before
    // dynamic initialization.
    //
    // In order to guarantee this, StaticStorage is a trivial type per C++11 rules. Because of this, StaticStorage
    // must have a constexpr default constructor.

    // Visual Studio 2013 does not yet support constexpr constructors, so the constructors must be compiler-generated.

    template<class T>
    class StaticStorage
    {
    public:
#ifndef _MSC_VER                                                                                                                        //TICS !POR#021
        constexpr StaticStorage() = default;
        StaticStorage(const StaticStorage&) = delete;
        StaticStorage operator=(const StaticStorage&) = delete;
#endif
        ~StaticStorage() = default;

        template<class... Args>
            void Construct(Args&&... args);
        template<class U, class... Args>
            void Construct(std::initializer_list<U> list, Args&&... args);

        void Destruct();

        T& operator*();
        const T& operator*() const;
        T* operator->();
        const T* operator->() const;

    private:
        typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type data;
    };

    template<class T, std::size_t ExtraSize, class AlignAs = uint64_t>
    class StaticStorageForPolymorphicObjects
    {
        static_assert(std::has_virtual_destructor<T>::value, "T needs to have a virtual destructor");

    public:
#ifndef _MSC_VER                                                                                                                            //TICS !POR#021
        constexpr StaticStorageForPolymorphicObjects() = default;
        StaticStorageForPolymorphicObjects(const StaticStorageForPolymorphicObjects&) = delete;
        StaticStorageForPolymorphicObjects operator=(const StaticStorageForPolymorphicObjects&) = delete;
#endif
        ~StaticStorageForPolymorphicObjects() = default;

        template<class U, class... Args>
            void Construct(Args&&... args);

        void Destruct();

        T& operator*();
        const T& operator*() const;
        T* operator->();
        const T* operator->() const;

    private:
        typename std::aligned_storage<sizeof(T) + ExtraSize, std::alignment_of<AlignAs>::value>::type data;
    };

    template<class Base, class... Derived>
    using StaticStorageForInheritanceTree = StaticStorageForPolymorphicObjects<Base, MaxSizeOfTypes<Derived...>::value - sizeof(Base), typename MaxAlignmentType<Derived...>::Type>;

    ////    Implementation    ////

    template<class T>
    template<class... Args>
    void StaticStorage<T>::Construct(Args&&... args)
    {
        new(&data) T(std::forward<Args>(args)...);
    }

    template<class T>
    template<class U, class... Args>
    void StaticStorage<T>::Construct(std::initializer_list<U> list, Args&&... args)
    {
        new(&data) T(list, std::forward<Args>(args)...);
    }

    template<class T>
    void StaticStorage<T>::Destruct()
    {
        reinterpret_cast<T&>(data).~T();
    }

    template<class T>
    T& StaticStorage<T>::operator*()
    {
        return reinterpret_cast<T&>(data);
    }

    template<class T>
    const T& StaticStorage<T>::operator*() const
    {
        return reinterpret_cast<const T&>(data);
    }

    template<class T>
    T* StaticStorage<T>::operator->()
    {
        return reinterpret_cast<T*>(&data);
    }

    template<class T>
    const T* StaticStorage<T>::operator->() const
    {
        return reinterpret_cast<const T*>(&data);
    }

    template<class T, std::size_t ExtraSize, class AlignAs>
    template<class U, class... Args>
    void StaticStorageForPolymorphicObjects<T, ExtraSize, AlignAs>::Construct(Args&&... args)
    {
        static_assert(std::is_base_of<T, U>::value, "Constructed type needs to be derived from T");
        static_assert(std::alignment_of<U>::value <= sizeof(AlignAs), "Alignment of U is larger than alignment of this function");
        static_assert(sizeof(U) <= sizeof(T) + ExtraSize, "Not enough static storage availabe for construction of derived type");
        new(&data) U(std::forward<Args>(args)...);
    }

    template<class T, std::size_t ExtraSize, class AlignAs>
    void StaticStorageForPolymorphicObjects<T, ExtraSize, AlignAs>::Destruct()
    {
        reinterpret_cast<T&>(data).~T();
    }

    template<class T, std::size_t ExtraSize, class AlignAs>
    T& StaticStorageForPolymorphicObjects<T, ExtraSize, AlignAs>::operator*()
    {
        return reinterpret_cast<T&>(data);
    }

    template<class T, std::size_t ExtraSize, class AlignAs>
    const T& StaticStorageForPolymorphicObjects<T, ExtraSize, AlignAs>::operator*() const
    {
        return reinterpret_cast<const T&>(data);
    }

    template<class T, std::size_t ExtraSize, class AlignAs>
    T* StaticStorageForPolymorphicObjects<T, ExtraSize, AlignAs>::operator->()
    {
        return reinterpret_cast<T*>(&data);
    }

    template<class T, std::size_t ExtraSize, class AlignAs>
    const T* StaticStorageForPolymorphicObjects<T, ExtraSize, AlignAs>::operator->() const
    {
        return reinterpret_cast<const T*>(&data);
    }
}

#endif
