#ifndef INFRA_PROXY_CREATOR_HPP
#define INFRA_PROXY_CREATOR_HPP

#include "infra/util/Function.hpp"
#include "infra/util/Optional.hpp"

namespace infra
{
    template<class T, class... ConstructionArgs>
    class CreatorBase;

    template<class T, class... ConstructionArgs>
    class ProxyCreator
    {
    public:
        ProxyCreator(CreatorBase<T, ConstructionArgs...>& creator, ConstructionArgs... args);
        ~ProxyCreator();

        T& operator*();
        const T& operator*() const;
        T* operator->();
        const T* operator->() const;

    private:
        CreatorBase<T, ConstructionArgs...>& creator;
    };

    template<class T, class... ConstructionArgs>
    class DelayedProxyCreator
    {
    public:
        explicit DelayedProxyCreator(CreatorBase<T, ConstructionArgs...>& creator);
        ~DelayedProxyCreator();

        void Emplace(ConstructionArgs... args);

        T& operator*();
        const T& operator*() const;
        T* operator->();
        const T* operator->() const;

    private:
        CreatorBase<T, ConstructionArgs...>& creator;
    };

    template<class T, class... ConstructionArgs>
    class CreatorBase
    {
    protected:
        CreatorBase() = default;
        ~CreatorBase() = default;

    protected:
        virtual T& Get() = 0;
        virtual const T& Get() const = 0;

        virtual void Emplace(ConstructionArgs... args) = 0;
        virtual void Destroy() = 0;

    private:
        template<class T2, class... ConstructionArgs2>
        friend class ProxyCreator;

        template<class T2, class... ConstructionArgs2>
        friend class DelayedProxyCreator;
    };

    template<class T, class U, class... ConstructionArgs>
    class Creator
        : public CreatorBase<T, ConstructionArgs...>
    {
    public:
        Creator();
        explicit Creator(infra::Function<void(infra::Optional<U>&, ConstructionArgs...)> emplaceFunction);

        virtual void Emplace(ConstructionArgs... args) override;
        virtual void Destroy() override;

        U& operator*();
        const U& operator*() const;
        U* operator->();
        const U* operator->() const;

    protected:
        virtual U& Get() override;
        virtual const U& Get() const override;

    private:
        infra::Optional<U> object;
        infra::Function<void(infra::Optional<U>& object, ConstructionArgs... args)> emplaceFunction;
    };

    ////    Implementation    ////

    template<class T, class... ConstructionArgs>
    ProxyCreator<T, ConstructionArgs...>::ProxyCreator(CreatorBase<T, ConstructionArgs...>& creator, ConstructionArgs... args)
        : creator(creator)
    {
        creator.Emplace(args...);
    }

    template<class T, class... ConstructionArgs>
    ProxyCreator<T, ConstructionArgs...>::~ProxyCreator()
    {
        creator.Destroy();
    }

    template<class T, class... ConstructionArgs>
    T& ProxyCreator<T, ConstructionArgs...>::operator*()
    {
        return creator.Get();
    }

    template<class T, class... ConstructionArgs>
    const T& ProxyCreator<T, ConstructionArgs...>::operator*() const
    {
        return creator.Get();
    }

    template<class T, class... ConstructionArgs>
    T* ProxyCreator<T, ConstructionArgs...>::operator->()
    {
        return &creator.Get();
    }

    template<class T, class... ConstructionArgs>
    const T* ProxyCreator<T, ConstructionArgs...>::operator->() const
    {
        return &creator.Get();
    }

    template<class T, class... ConstructionArgs>
    DelayedProxyCreator<T, ConstructionArgs...>::DelayedProxyCreator(CreatorBase<T, ConstructionArgs...>& creator)
        : creator(creator)
    {}

    template<class T, class... ConstructionArgs>
    DelayedProxyCreator<T, ConstructionArgs...>::~DelayedProxyCreator()
    {
        creator.Destroy();
    }

    template<class T, class... ConstructionArgs>
    void DelayedProxyCreator<T, ConstructionArgs...>::Emplace(ConstructionArgs... args)
    {
        creator.Emplace(args...);
    }

    template<class T, class... ConstructionArgs>
    T& DelayedProxyCreator<T, ConstructionArgs...>::operator*()
    {
        return creator.Get();
    }

    template<class T, class... ConstructionArgs>
    const T& DelayedProxyCreator<T, ConstructionArgs...>::operator*() const
    {
        return creator.Get();
    }

    template<class T, class... ConstructionArgs>
    T* DelayedProxyCreator<T, ConstructionArgs...>::operator->()
    {
        return &creator.Get();
    }

    template<class T, class... ConstructionArgs>
    const T* DelayedProxyCreator<T, ConstructionArgs...>::operator->() const
    {
        return &creator.Get();
    }

    template<class T, class U, class... ConstructionArgs>
    Creator<T, U, ConstructionArgs...>::Creator()
    {
        emplaceFunction = [](infra::Optional<U>& object, ConstructionArgs... args) { object.Emplace(args...); };
    }
        
    template<class T, class U, class... ConstructionArgs>
    Creator<T, U, ConstructionArgs...>::Creator(infra::Function<void(infra::Optional<U>&, ConstructionArgs...)> emplaceFunction)
        : emplaceFunction(emplaceFunction)
    {}

    template<class T, class U, class... ConstructionArgs>
    void Creator<T, U, ConstructionArgs...>::Emplace(ConstructionArgs... args)
    {
        assert(!object);
        emplaceFunction(object, args...);
    }

    template<class T, class U, class... ConstructionArgs>
    void Creator<T, U, ConstructionArgs...>::Destroy()
    {
        object = none;
    }

    template<class T, class U, class... ConstructionArgs>
    U& Creator<T, U, ConstructionArgs...>::operator*()
    {
        return Get();
    }

    template<class T, class U, class... ConstructionArgs>
    const U& Creator<T, U, ConstructionArgs...>::operator*() const
    {
        return Get();
    }

    template<class T, class U, class... ConstructionArgs>
    U* Creator<T, U, ConstructionArgs...>::operator->()
    {
        return &Get();
    }

    template<class T, class U, class... ConstructionArgs>
    const U* Creator<T, U, ConstructionArgs...>::operator->() const
    {
        return &Get();
    }

    template<class T, class U, class... ConstructionArgs>
    U& Creator<T, U, ConstructionArgs...>::Get()
    {
        return *object;
    }

    template<class T, class U, class... ConstructionArgs>
    const U& Creator<T, U, ConstructionArgs...>::Get() const
    {
        return *object;
    }
}

#endif
