#ifndef SERVICES_CYCLIC_FLASH_STORAGE_IMPL_HPP
#define SERVICES_CYCLIC_FLASH_STORAGE_IMPL_HPP

#include "hal/interfaces/public/Flash.hpp"
#include "services/util/public/CyclicFlashStorage.hpp"

namespace services
{

    class CyclicFlashStorageImpl : private CyclicFlashStorage
    {
    public:
        CyclicFlashStorageImpl(hal::Flash& flash);
        CyclicFlashStorageImpl(const CyclicFlashStorageImpl& other) = delete;
        CyclicFlashStorageImpl& operator=(const CyclicFlashStorageImpl& other) = delete;

        virtual void Initialize(infra::Function<void()> onDone) override;
        virtual void Add(infra::ConstByteRange range, infra::Function<void()> onDone) override;
        virtual void Clear(infra::Function<void()> onDone) override;

        class Iterator;
        Iterator Begin() const;

    protected:
    private:
        hal::Flash& flash;        
    };
};

#endif