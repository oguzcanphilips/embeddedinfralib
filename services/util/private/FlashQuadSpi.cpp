#include "services/util/public/FlashQuadSpi.hpp"

namespace services
{

    const uint8_t FlashQuadSpi::commandPageProgram = 0x32;
    const uint8_t FlashQuadSpi::commandReadData = 0x0B;
    const uint8_t FlashQuadSpi::commandReadStatusRegister = 0x05;
    const uint8_t FlashQuadSpi::commandReadFlagStatusRegister = 0x70;
    const uint8_t FlashQuadSpi::commandWriteEnable = 0x06;
    const uint8_t FlashQuadSpi::commandEraseSubSector = 0x20;
    const uint8_t FlashQuadSpi::commandEraseSector = 0xd8;
    const uint8_t FlashQuadSpi::commandEraseBulk = 0xc7;
    const uint8_t FlashQuadSpi::commandWriteEnhancedVolatileRegister = 0x61;
    const uint8_t FlashQuadSpi::commandReadEnhancedVolatileRegister = 0x65;

    const uint8_t FlashQuadSpi::volatileRegisterForQuadSpeed = 0x5f;

    FlashQuadSpi::FlashQuadSpi(hal::QuadSpi& spi, infra::Function<void()> onInitialized, uint32_t numberOfSubSectors)
        : hal::FlashHomogeneous(numberOfSubSectors, sizeSubSector)
        , spi(spi)
        , onInitialized(onInitialized)
    {
        sequencer.Load([this]()
        {
            sequencer.Step([this]() { initDelayTimer.Start(std::chrono::milliseconds(1), [this]() { sequencer.Continue(); }); });
            sequencer.Step([this]() { WriteEnableSingleSpeed(); });
            sequencer.Step([this]() { SwitchToQuadSpeed(); });
            sequencer.Step([this]() { this->onInitialized(); });
        });
    }

    void FlashQuadSpi::WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone)
    {
        this->onDone = onDone;
        this->buffer = buffer;
        this->address = address;

        WriteBufferSequence();
    }

    void FlashQuadSpi::ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone)
    {
        const hal::QuadSpi::Header header{ infra::MakeOptional(commandReadData), ConvertAddress(address), {}, 10 };

        spi.ReceiveData(header, buffer, hal::QuadSpi::Lines::QuadSpeed(), onDone);
    }

    void FlashQuadSpi::EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone)
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
            sequencer.Execute([this]() { this->onDone(); });
        });
    }

    void FlashQuadSpi::WriteBufferSequence()
    {
        sequencer.Load([this]()
        {
            sequencer.While([this]() { return !this->buffer.empty(); });
            sequencer.Step([this]() { WriteEnable(); });
            sequencer.Step([this]() { PageProgram(); });
            sequencer.Step([this]() { HoldWhileWriteInProgress(); });
            sequencer.EndWhile();
            sequencer.Execute([this]() { this->onDone(); });
        });
    }

    infra::BoundedVector<uint8_t>::WithMaxSize<4> FlashQuadSpi::ConvertAddress(uint32_t address) const
    {
        return hal::QuadSpi::AddressToVector(address, 3);
    }

    void FlashQuadSpi::WriteEnableSingleSpeed()
    {
        static const hal::QuadSpi::Header writeEnableHeader{ infra::MakeOptional(commandWriteEnable), {}, {}, 0 };
        spi.SendData(writeEnableHeader, {}, hal::QuadSpi::Lines::SingleSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpi::SwitchToQuadSpeed()
    {
        static const hal::QuadSpi::Header writeVolatileRegisterHeader{ infra::MakeOptional(commandWriteEnhancedVolatileRegister), {}, {}, 0 };
        spi.SendData(writeVolatileRegisterHeader, infra::MakeByteRange(volatileRegisterForQuadSpeed), hal::QuadSpi::Lines::SingleSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpi::WriteEnable()
    {
        static const hal::QuadSpi::Header writeEnableHeader{ infra::MakeOptional(commandWriteEnable), {}, {}, 0 };
        spi.SendData(writeEnableHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpi::PageProgram()
    {
        hal::QuadSpi::Header pageProgramHeader{ infra::MakeOptional(commandPageProgram), ConvertAddress(address), {}, 0 };

        infra::ConstByteRange currentBuffer = infra::Head(buffer, sizePage - AddressOffsetInSector(address) % sizePage);
        buffer.pop_front(currentBuffer.size());
        address += currentBuffer.size();

        spi.SendData(pageProgramHeader, currentBuffer, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpi::EraseSomeSectors(uint32_t endIndex)
    {
        if (sectorIndex == 0 && endIndex == NumberOfSectors())
        {
            SendEraseBulk();
            sectorIndex += NumberOfSectors();
        }
        else if (sectorIndex % (sizeSector / sizeSubSector) == 0 && sectorIndex + sizeSector / sizeSubSector <= endIndex)
        {
            SendEraseSector(sectorIndex);
            sectorIndex += sizeSector / sizeSubSector;
        }
        else
        {
            SendEraseSubSector(sectorIndex);
            ++sectorIndex;
        }
    }

    void FlashQuadSpi::SendEraseSubSector(uint32_t subSectorIndex)
    {
        hal::QuadSpi::Header eraseSubSectorHeader{ infra::MakeOptional(commandEraseSubSector), ConvertAddress(AddressOfSector(subSectorIndex)), {}, 0 };
        spi.SendData(eraseSubSectorHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpi::SendEraseSector(uint32_t subSectorIndex)
    {
        hal::QuadSpi::Header eraseSectorHeader{ infra::MakeOptional(commandEraseSector), ConvertAddress(AddressOfSector(subSectorIndex)), {}, 0 };
        spi.SendData(eraseSectorHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpi::SendEraseBulk()
    {
        static const hal::QuadSpi::Header eraseBulkHeader{ infra::MakeOptional(commandEraseBulk), {}, {}, 0 };
        spi.SendData(eraseBulkHeader, {}, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }

    void FlashQuadSpi::HoldWhileWriteInProgress()
    {
        static const hal::QuadSpi::Header pollWriteInProgressHeader{ infra::MakeOptional(commandReadStatusRegister), {}, {}, 0 };
        spi.PollStatus(pollWriteInProgressHeader, 1, 0, statusFlagWriteInProgress, hal::QuadSpi::Lines::QuadSpeed(), [this]() { sequencer.Continue(); });
    }
}
