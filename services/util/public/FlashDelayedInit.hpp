#ifndef SERVICES_FLASH_DELAYED_INIT_HPP
#define SERVICES_FLASH_DELAYED_INIT_HPP

#include "hal/interfaces/public/Flash.hpp"
#include "infra/util/public/AutoResetFunction.hpp"

namespace services
{
    class FlashDelayedInit
        : public hal::Flash
    {
    public:
        FlashDelayedInit(hal::Flash& master);

    public:
        virtual uint32_t NumberOfSectors() const override;
        virtual uint32_t SizeOfSector(uint32_t sectorIndex) const override;

        virtual uint32_t SectorOfAddress(uint32_t address) const override;
        virtual uint32_t AddressOfSector(uint32_t sectorIndex) const override;

        virtual void WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone) override;

        void Initialized();

    private:
        hal::Flash& master;
        bool initialized = false;

        infra::AutoResetFunction<void(), INFRA_DEFAULT_FUNCTION_EXTRA_SIZE + 20> onInitialized;
    };
}

#endif
