#ifndef INFRA_REFERENCE_COUNTED_SINGLETONS_HPP
#define INFRA_REFERENCE_COUNTED_SINGLETONS_HPP

#include "infra/util/public/StaticStorage.hpp"
#include "infra/util/public/Variant.hpp"
#include "infra/util/public/VariadicTemplates.hpp"

// Two classes are available: ReferenceCountedSingleton, and ReferenceCountedSingletons (plural)
// These classes help creating singletons that are constructed and destroyed based on a reference count.
// This reference count is controlled by creating objects of a nested Access type, via which the access
// to the singleton is controlled as well.

// Example for ReferenceCountedSingletons:
//
// Suppose we have a class Dma for which two singletons exists, and a class A which uses the second DMA
// In this case, we want two instances of Dma to be constructed, one for which the constructor is passed
// the value 1, the other is constructed with a 2.
//
// class Dma
//     : ReferenceCountedSingletons<Dma>
// {
//     Dma(std::size_t index) { initialize dma1 or dma2, based on 'index' }
//     void DoSomething();
// };
//
// struct Dma1: Dma::Name<Dma1, 1> {};
// struct Dma2: Dma::Name<Dma2, 2> {};
//
// class A
// {
//     Dma2::Access dma2;
//
//     void f()
//     {
//         dma2->DoSomething();
//     }
// };

namespace infra
{

    template<class CRTP>
    class ReferenceCountedSingletons
    {
    public:
        template<class N>
        class Access
        {
        public:
            Access();
            ~Access();

            Access(const Access&) = delete;
            Access& operator=(const Access&) = delete;

            CRTP& operator*();
            const CRTP& operator*() const;
            CRTP* operator->();
            const CRTP* operator->() const;
        };

        class NameBase
        {
        public:
            NameBase() = default;
            NameBase(const NameBase&) = delete;
            NameBase& operator=(const NameBase&) = delete;

            uint32_t counter;
            infra::StaticStorage<CRTP> storage;
        };

        template<class NameCRTP, std::size_t I>
        class Name
            : public NameBase
        {
        public:
            typedef CRTP ObjectType;
            typedef typename infra::ReferenceCountedSingletons<CRTP>::template Access<NameCRTP> Access;

            static const std::size_t identifier = I;

            static Name& Instance();

        private:
            Name() = default;

            static Name sInstance;
        };
    };

    template<class CRTP>
    template<class NameCRTP, std::size_t I>
    typename ReferenceCountedSingletons<CRTP>::template Name<NameCRTP, I> ReferenceCountedSingletons<CRTP>::Name<NameCRTP, I>::sInstance;

    template<class... Names>
    class IndexedAccess
    {
    private:
        typedef typename Front<Names...>::Type::ObjectType ObjectType;
        
    public:
        IndexedAccess(std::size_t index);

        ObjectType& operator*();
        const ObjectType& operator*() const;
        ObjectType* operator->();
        const ObjectType* operator->() const;

    private:
        template<class... AccessNames>
        struct MakeVariantHelper
        {
            typedef List<> Type;
        };

        template<class Head, class... Tail>
        struct MakeVariantHelper<Head, Tail...>
        {
            typedef typename ListPushFront<typename Head::Access, typename MakeVariantHelper<Tail...>::Type>::Type Type;
        };

        typename MakeVariantOver<typename MakeVariantHelper<Names...>::Type>::Type object;
    };

    //// Implementation ////

    template<class CRTP>
    template<class NameCRTP, std::size_t I>
#ifdef _MSC_VER
    typename ReferenceCountedSingletons<CRTP>::Name<NameCRTP, I>& ReferenceCountedSingletons<CRTP>::Name<NameCRTP, I>::Instance()
#else
    typename ReferenceCountedSingletons<CRTP>::template Name<NameCRTP, I>& ReferenceCountedSingletons<CRTP>::Name<NameCRTP, I>::Instance()
#endif
    {
        return sInstance;
    }

    template<class CRTP>
    template<class N>
    ReferenceCountedSingletons<CRTP>::Access<N>::Access()
    {
        if (N::Instance().counter == 0)
            N::Instance().storage.Construct(std::size_t(N::identifier));

        ++N::Instance().counter;
    }

    template<class CRTP>
    template<class N>
    ReferenceCountedSingletons<CRTP>::Access<N>::~Access()
    {
        --N::Instance().counter;

        if (N::Instance().counter == 0)
            N::Instance().storage.Destruct();
    }

    template<class CRTP>
    template<class N>
    CRTP& ReferenceCountedSingletons<CRTP>::Access<N>::operator*()
    {
        return *N::Instance().storage;
    }

    template<class CRTP>
    template<class N>
    const CRTP& ReferenceCountedSingletons<CRTP>::Access<N>::operator*() const
    {
        return *N::Instance().storage;
    }

    template<class CRTP>
    template<class N>
    CRTP* ReferenceCountedSingletons<CRTP>::Access<N>::operator->()
    {
        return &*N::Instance().storage;
    }

    template<class CRTP>
    template<class N>
    const CRTP* ReferenceCountedSingletons<CRTP>::Access<N>::operator->() const
    {
        return &*N::Instance().storage;
    }

    template<class... Names>
    IndexedAccess<Names...>::IndexedAccess(std::size_t index)
        : object(infra::atIndex, index)
    {}

    template<class ObjectType>
    struct DereferenceVisitor
        : public infra::StaticVisitor<ObjectType&>
    {
        template<class T>
            ObjectType& operator()(T& access);
    };

    template<class ObjectType>
    template<class T>
    ObjectType& DereferenceVisitor<ObjectType>::operator()(T& access)
    {
        return *access;
    }

    template<class... Names>
    typename IndexedAccess<Names...>::ObjectType& IndexedAccess<Names...>::operator*()
    {
        DereferenceVisitor<ObjectType> visitor;
        return infra::ApplyVisitor(visitor, object);
    }

    template<class... Names>
    const typename IndexedAccess<Names...>::ObjectType& IndexedAccess<Names...>::operator*() const
    {
        DereferenceVisitor<const ObjectType> visitor;
        return infra::ApplyVisitor(visitor, object);
    }

    template<class... Names>
    typename IndexedAccess<Names...>::ObjectType* IndexedAccess<Names...>::operator->()
    {
        return &**this;
    }

    template<class... Names>
    const typename IndexedAccess<Names...>::ObjectType* IndexedAccess<Names...>::operator->() const
    {
        return &**this;
    }
}

#endif
