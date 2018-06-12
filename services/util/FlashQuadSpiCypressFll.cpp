#include "infra/event/EventDispatcher.hpp"
#include "services/util/FlashQuadSpiCypressFll.hpp"

namespace services
{
    const uint8_t FlashQuadSpiCypressFll::commandPageProgram = 0x02;
    const uint8_t FlashQuadSpiCypressFll::commandReadData = 0xeb;
    const uint8_t FlashQuadSpiCypressFll::commandReadStatusRegister = 0x05;
    const uint8_t FlashQuadSpiCypressFll::commandWriteEnable = 0x06;
    const uint8_t FlashQuadSpiCypressFll::commandEraseSector = 0x20;
    const uint8_t FlashQuadSpiCypressFll::commandEraseHalfBlock = 0x52;
    const uint8_t FlashQuadSpiCypressFll::commandEraseBlock = 0xd8;
    const uint8_t FlashQuadSpiCypressFll::commandEraseChip = 0x60;
    const uint8_t FlashQuadSpiCypressFll::commandEnterQpi = 0x38;
    const uint8_t FlashQuadSpiCypressFll::commandExitQpi = 0xf5;

    FlashQuadSpiCypressFll::FlashQuadSpiCypressFll(hal::QuadSpi& spi, infra::Function<void()> onInitialized, uint32_t numberOfSubSectors)
        : hal::FlashHomogeneous(numberOfSubSectors, sizeSector)
        , spi(spi)
        , onInitialized(onInitialized)
    {
        sequencer.Load([this]()
        {
            sequencer.Step([this]() { initDelayTimer.Start(std::chrono::milliseconds(100), [this]() { sequencer.Continue(); }); });
            sequencer.Step([this]() { SwitchToQuadSpeed(); });
            sequencer.Step([this]() { infra::EventDispatcher::Instance().Schedule([this]() { this->onInitialized(); }); });
        });
    }

    void FlashQuadSpiCypressFll::WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone)
    {
        this->onDone = onDone;
        this->buffer = buffer;
        this->address = address;

        WriteBufferSequence();
    }

    void FlashQuadSpiCypressFll::ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone)
    {
        const hal::QuadSpi::Header header{ infra::MakeOptional(commandReadData), hal::QuadSpi::AddressToVector(address << 8, 4), {}, 8 };

        spi.ReceiveData(header, buffer, hal::QuadSpi::Lines::QuadSpeed(), onDone);
    }

    void FlashQuadSpiCypressFll::EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone)
    {
        this->onDone = onDone;
        sectorIndex = beginIndex;
        sequencer.Load([this, endIndex]()
        {
            sequencer.While([this, endIndex]() { return sectorIndex != endIndex; });
                sequencer.Step([this]() { WriteEnable(); });
                sequencer.Step([this, endIndex]() { EraseSomeSectors(endIndex); });
                sequencer.Step([this]() { HoldWhileWriteInProgress(); });
            sequencer.EndWhile();
            sequencer.Execute([this]() { infra::EventDispatcher::Instance().Schedule([this]() { this->onDone(); }); });
        });
    }

    void FlashQuadSpiCypressFll::SwitchToSingleSpeed(infra::Function<void()> onDone)
    {
        static const hal::QuadSpi::Header exitQpiHeader{ infra::MakeOptional(commandExitQpi), {}, {}, 0 };
        spi.SendData(exitQpiHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), onDone);
    }

    void FlashQuadSpiCypressFll::WriteBufferSequence()
    {
        sequencer.Load([this]()
        {
            sequencer.While([this]() { return !this->buffer.empty(); });
            sequencer.Step([this]() { WriteEnable(); });
            sequencer.Step([this]() { PageProgram(); });
            sequencer.Step([this]() { HoldWhileWriteInProgress(); });
            sequencer.EndWhile();
            sequencer.Execute([this]() { infra::EventDispatcher::Instance().Schedule([this]() { this->onDone(); }); });
        });
    }

    infra::BoundedVector<uint8_t>::WithMaxSize<4> FlashQuadSpiCypressFll::ConvertAddress(uint32_t address) const
    {
        return hal::QuadSpi::AddressToVector(address, 3);
    }

    void FlashQuadSpiCypressFll::SwitchToQuadSpeed()
    {
        static const hal::QuadSpi::Header enterQpiHeader{ infra::MakeOptional(commandEnterQpi), {}, {}, 0 };
        spi.SendData(enterQpiHeader, {}, hal::QuadSpi::Lines::SingleSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpiCypressFll::WriteEnable()
    {
        static const hal::QuadSpi::Header writeEnableHeader{ infra::MakeOptional(commandWriteEnable), {}, {}, 0 };
        spi.SendData(writeEnableHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpiCypressFll::PageProgram()
    {
        hal::QuadSpi::Header pageProgramHeader{ infra::MakeOptional(commandPageProgram), ConvertAddress(address), {}, 0 };

        infra::ConstByteRange currentBuffer = infra::Head(buffer, sizePage - AddressOffsetInSector(address) % sizePage);
        buffer.pop_front(currentBuffer.size());
        address += currentBuffer.size();

        spi.SendData(pageProgramHeader, currentBuffer, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpiCypressFll::EraseSomeSectors(uint32_t endIndex)
    {
        if (sectorIndex == 0 && endIndex == NumberOfSectors())
        {
            SendEraseChip();
            sectorIndex += NumberOfSectors();
        }
        else if (sectorIndex % (sizeBlock / sizeSector) == 0 && sectorIndex + sizeBlock / sizeSector <= endIndex)
        {
            SendEraseBlock(sectorIndex);
            sectorIndex += sizeBlock / sizeSector;
        }
        else if (sectorIndex % (sizeHalfBlock / sizeSector) == 0 && sectorIndex + sizeHalfBlock / sizeSector <= endIndex)
        {
            SendEraseHalfBlock(sectorIndex);
            sectorIndex += sizeHalfBlock / sizeSector;
        }
        else
        {
            SendEraseSector(sectorIndex);
            ++sectorIndex;
        }
    }

    void FlashQuadSpiCypressFll::SendEraseSector(uint32_t sectorIndex)
    {
        hal::QuadSpi::Header eraseSectorHeader{ infra::MakeOptional(commandEraseSector), ConvertAddress(AddressOfSector(sectorIndex)), {}, 0 };
        spi.SendData(eraseSectorHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpiCypressFll::SendEraseHalfBlock(uint32_t sectorIndex)
    {
        hal::QuadSpi::Header eraseHalfBlockHeader{ infra::MakeOptional(commandEraseHalfBlock), ConvertAddress(AddressOfSector(sectorIndex)), {}, 0 };
        spi.SendData(eraseHalfBlockHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpiCypressFll::SendEraseBlock(uint32_t sectorIndex)
    {
        hal::QuadSpi::Header eraseBlockHeader{ infra::MakeOptional(commandEraseBlock), ConvertAddress(AddressOfSector(sectorIndex)), {}, 0 };
        spi.SendData(eraseBlockHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpiCypressFll::SendEraseChip()
    {
        static const hal::QuadSpi::Header eraseChipHeader{ infra::MakeOptional(commandEraseChip), {}, {}, 0 };
        spi.SendData(eraseChipHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpiCypressFll::HoldWhileWriteInProgress()
    {
        static const hal::QuadSpi::Header pollWriteInProgressHeader{ infra::MakeOptional(commandReadStatusRegister), {}, {}, 0 };
        spi.PollStatus(pollWriteInProgressHeader, 1, 0, statusFlagWriteInProgress, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }
}
