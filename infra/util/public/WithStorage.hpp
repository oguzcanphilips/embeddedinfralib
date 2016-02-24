#ifndef INFRA_WITH_STORAGE_HPP
#define INFRA_WITH_STORAGE_HPP

#include "infra/util/public/Optional.hpp"  // For InPlace
#include <utility>

namespace infra
{
    template<class Base, class StorageType>
    class WithStorage
        : public Base
    {
    public:
        template<class StorageArg, class... Args>
            WithStorage(InPlace, StorageArg&& storageArg, Args&&... args);
        template<class... Args>
            WithStorage(Args&&... args);
        template<class T, class... Args>
            WithStorage(std::initializer_list<T> initializerList, Args&&... args);

        using Base::operator=;

        const StorageType& Storage() const;
        StorageType& Storage();

    private:
        StorageType storage;
    };

    ////    Implementation    ////

    template<class Base, class StorageType>
    template<class StorageArg, class... Args>
    WithStorage<Base, StorageType>::WithStorage(InPlace, StorageArg&& storageArg, Args&&... args)
        : Base(storage, std::forward<Args>(args)...)
        , storage(std::forward<StorageArg>(storageArg))
    {}

    template<class Base, class StorageType>
    template<class... Args>
    WithStorage<Base, StorageType>::WithStorage(Args&&... args)
        : Base(storage, std::forward<Args>(args)...)
    {}

    template<class Base, class StorageType>
    template<class T, class... Args>
    WithStorage<Base, StorageType>::WithStorage(std::initializer_list<T> initializerList, Args&&... args)
        : Base(storage, initializerList, std::forward<Args>(args)...)
    {}

    template<class Base, class StorageType>
    const StorageType& WithStorage<Base, StorageType>::Storage() const
    {
        return storage;
    }

    template<class Base, class StorageType>
    StorageType& WithStorage<Base, StorageType>::Storage()
    {
        return storage;
    }
}

#endif
