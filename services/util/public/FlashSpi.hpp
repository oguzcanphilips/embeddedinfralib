#ifndef SERVICES_FLASH_SPI_HPP
#define SERVICES_FLASH_SPI_HPP

#include "hal/interfaces/public/Spi.hpp"
#include "infra/timer/public/Timer.hpp"
#include "infra/util/public/AutoResetFunction.hpp"
#include "infra/util/public/Sequencer.hpp"
#include "hal/interfaces/public/FlashHomogeneous.hpp"

namespace services
{
    class FlashSpi
        : public hal::FlashHomogeneous
    {
    public:
        static const uint8_t commandPageProgram;
        static const uint8_t commandReadData;
        static const uint8_t commandReadStatusRegister;
        static const uint8_t commandWriteEnable;
        static const uint8_t commandEraseSubSector;
        static const uint8_t commandEraseSector;
        static const uint8_t commandEraseBulk;

        static const uint32_t sizeSector = 65536;
        static const uint32_t sizeSubSector = 4096;
        static const uint32_t sizePage = 256;

        static const uint8_t statusFlagWriteInProgress = 1;

        explicit FlashSpi(hal::SpiMaster& spi, uint32_t numberOfSubSectors = 512);

    public:
        virtual void WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone) override;

    private:
        std::array<uint8_t, 3> ConvertAddress(uint32_t address) const;

        void WriteEnable();
        void PageProgram();
        void EraseSomeSectors(uint32_t endIndex);
        void SendEraseSubSector(uint32_t subSectorIndex);
        void SendEraseSector(uint32_t subSectorIndex);
        void SendEraseBulk();
        void HoldWhileWriteInProgress();
        void ReadStatusRegister();

    private:
        hal::SpiMaster& spi;
        uint32_t numberOfSubSectors;
        infra::Sequencer sequencer;
        infra::TimerSingleShot delayTimer;
        infra::AutoResetFunction<void()> onDone;
        infra::ConstByteRange buffer;
        infra::ConstByteRange writeBuffer;
        infra::ByteRange readBuffer;
        uint32_t address;
        uint32_t sectorIndex;
        uint8_t statusRegister = 0;

        struct InstructionAndAddress
        {
            uint8_t instruction;
            std::array<uint8_t, 3> address;
        } instructionAndAddress;
    };
}

#endif
