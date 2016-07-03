#ifndef HAL_STUB_FLASH_MOCK_HPP
#define HAL_STUB_FLASH_MOCK_HPP

#include "gmock/gmock.h"
#include "hal/interfaces/public/Flash.hpp"

namespace hal
{
    class FlashMock
        : public hal::Flash
    {
    public:
        FlashMock(uint32_t numberOfSectors = 4, uint32_t sizeOfSectors = 16);

        uint32_t NumberOfSectors() const override;
        uint32_t SizeOfSector(uint32_t sectorIndex) const override;

        uint32_t SectorOfAddress(uint32_t address) const override;
        uint32_t AddressOfSector(uint32_t sectorIndex) const override;

        void WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        void ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        void EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone) override;

        uint32_t numberOfSectors;
        uint32_t sizeOfSectors;

        MOCK_METHOD2(writeBufferMock, void(std::vector<uint8_t>, uint32_t));
        MOCK_METHOD1(readBufferMock, std::vector<uint8_t>(uint32_t));
        MOCK_METHOD2(eraseSectorsMock, void(uint32_t, uint32_t));

        infra::Function<void()> done;
    };
}

#endif
