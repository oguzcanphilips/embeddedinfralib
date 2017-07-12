#include "hal/interfaces/test_doubles/FlashMock.hpp"

namespace hal
{
    FlashMock::FlashMock(uint32_t numberOfSectors, uint32_t sizeOfSectors)
        : numberOfSectors(numberOfSectors)
        , sizeOfSectors(sizeOfSectors)
    {}

    uint32_t FlashMock::NumberOfSectors() const
    {
        return numberOfSectors;
    }

    uint32_t FlashMock::SizeOfSector(uint32_t sectorIndex) const
    {
        return sizeOfSectors;
    }

    uint32_t FlashMock::SectorOfAddress(uint32_t address) const
    {
        return address / sizeOfSectors;
    }

    uint32_t FlashMock::AddressOfSector(uint32_t sectorIndex) const
    {
        return sectorIndex * sizeOfSectors;
    }

    void FlashMock::WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone)
    {
        done = onDone;
        writeBufferMock(std::vector<uint8_t>(buffer.begin(), buffer.end()), address);
    }

    void FlashMock::ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone)
    {
        done = onDone;
        std::vector<uint8_t> result = readBufferMock(address);
        assert(result.size() == buffer.size());
        std::copy(result.begin(), result.end(), buffer.begin());
    }

    void FlashMock::EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone)
    {
        done = onDone;
        eraseSectorsMock(beginIndex, endIndex);
    }
}