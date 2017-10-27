#ifndef SERVICES_FLASH_QUAD_SPI_CYPRESS_FLL_HPP
#define SERVICES_FLASH_QUAD_SPI_CYPRESS_FLL_HPP

#include "hal/interfaces/QuadSpi.hpp"
#include "infra/timer/Timer.hpp"
#include "infra/util/AutoResetFunction.hpp"
#include "infra/util/Sequencer.hpp"
#include "hal/interfaces/FlashHomogeneous.hpp"

namespace services
{
    class FlashQuadSpiCypressFll
        : public hal::FlashHomogeneous
    {
    public:
        static const uint8_t commandPageProgram;
        static const uint8_t commandReadData;
        static const uint8_t commandReadStatusRegister;
        static const uint8_t commandWriteEnable;
        static const uint8_t commandEraseSector;
        static const uint8_t commandEraseHalfBlock;
        static const uint8_t commandEraseBlock;
        static const uint8_t commandEraseChip;
        static const uint8_t commandEnterQpi;

        static const uint32_t sizeBlock = 65536;
        static const uint32_t sizeHalfBlock = 32768;
        static const uint32_t sizeSector = 4096;
        static const uint32_t sizePage = 256;

        static const uint8_t statusFlagWriteInProgress = 1;

        FlashQuadSpiCypressFll(hal::QuadSpi& spi, infra::Function<void()> onInitialized, uint32_t numberOfSubSectors = 4096);

    public:
        virtual void WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone) override;

    private:
        void WriteBufferSequence();
        infra::BoundedVector<uint8_t>::WithMaxSize<4> ConvertAddress(uint32_t address) const;

        void SwitchToQuadSpeed();
        void WriteEnable();
        void PageProgram();
        void EraseSomeSectors(uint32_t endIndex);
        void SendEraseSector(uint32_t sectorIndex);
        void SendEraseHalfBlock(uint32_t sectorIndex);
        void SendEraseBlock(uint32_t sectorIndex);
        void SendEraseChip();
        void HoldWhileWriteInProgress();

    private:
        hal::QuadSpi& spi;
        infra::Function<void()> onInitialized;
        infra::Sequencer sequencer;
        infra::TimerSingleShot initDelayTimer;
        infra::AutoResetFunction<void()> onDone;
        infra::ConstByteRange buffer;
        uint32_t address = 0;
        uint32_t sectorIndex = 0;
    };
}

#endif
