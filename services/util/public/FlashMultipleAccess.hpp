#ifndef SERVICES_FLASH_MULTIPLE_ACCESS_HPP
#define SERVICES_FLASH_MULTIPLE_ACCESS_HPP

#include "hal/interfaces/public/Flash.hpp"
#include "infra/event/public/ClaimableResource.hpp"
#include "infra/util/public/AutoResetFunction.hpp"

namespace services
{
    class FlashMultipleAccessMaster
        : public hal::Flash
        , public infra::ClaimableResource
    {
    public:
        FlashMultipleAccessMaster(hal::Flash& master);

        virtual uint32_t NumberOfSectors() const override;
        virtual uint32_t SizeOfSector(uint32_t sectorIndex) const override;
        virtual uint32_t SectorOfAddress(uint32_t address) const override;
        virtual uint32_t AddressOfSector(uint32_t sectorIndex) const override;
        virtual void WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone) override;

    private:
        hal::Flash& master;
    };

    class FlashMultipleAccess
        : public hal::Flash
    {
    public:
        FlashMultipleAccess(FlashMultipleAccessMaster& master);

        virtual uint32_t NumberOfSectors() const override;
        virtual uint32_t SizeOfSector(uint32_t sectorIndex) const override;
        virtual uint32_t SectorOfAddress(uint32_t address) const override;
        virtual uint32_t AddressOfSector(uint32_t sectorIndex) const override;
        virtual void WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone) override;

    private:
        FlashMultipleAccessMaster& master;
        infra::ClaimableResource::Claimer::WithSize<40> claimer;
        infra::AutoResetFunction<void()> onDone;
    };
}

#endif