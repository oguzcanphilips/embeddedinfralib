#ifndef SERVICES_CYCLIC_FLASH_STORAGE_HPP
#define SERVICES_CYCLIC_FLASH_STORAGE_HPP

#include "hal/interfaces/public/Flash.hpp"

namespace services
{

    class CyclicFlashStorage
    {
    public:
        virtual void Initialize(infra::Function<void()> onDone) = 0;
        virtual void Add(infra::ConstByteRange range, infra::Function<void()> onDone) = 0;
        virtual void Clear(infra::Function<void()> onDone) = 0;

        class Iterator;
        Iterator Begin() const;

    protected:
        CyclicFlashStorage();
        CyclicFlashStorage(const CyclicFlashStorage& other) = delete;
        CyclicFlashStorage& operator=(const CyclicFlashStorage& other) = delete;
    };

    class CyclicFlashStorage::Iterator
    {
    public:
        friend class CyclicFlashStorage;
        Iterator(const Iterator& other);
        ~Iterator();
        Iterator& operator=(const Iterator& other);


        void GoToNext();
        void GoToFirstOfNextSector();

        void Read(infra::ByteRange buffer, uint32_t offset, infra::Function<void(infra::ByteRange result)> onDone);//const ????
        void Read(infra::ByteRange buffer, infra::Function<void(infra::ByteRange result)> onDone);

    private:
        explicit Iterator(const CyclicFlashStorage& store);
    };


}

#endif
