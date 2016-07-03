#ifndef SERVICES_FLASH_QUAD_SPI_HPP
#define SERVICES_FLASH_QUAD_SPI_HPP

#include "hal/interfaces/public/QuadSpi.hpp"
#include "infra/timer/public/Timer.hpp"
#include "infra/util/public/AutoResetFunction.hpp"
#include "infra/util/public/Sequencer.hpp"
#include "hal/interfaces/public/FlashHomogeneous.hpp"

namespace services
{
    class FlashQuadSpi
        : public hal::FlashHomogeneous
    {
    public:
        static const uint8_t commandPageProgram;
        static const uint8_t commandReadData;
        static const uint8_t commandReadStatusRegister;
        static const uint8_t commandReadFlagStatusRegister;
        static const uint8_t commandWriteEnable;
        static const uint8_t commandEraseSubSector;
        static const uint8_t commandEraseSector;
        static const uint8_t commandEraseBulk;
        static const uint8_t commandWriteEnhancedVolatileRegister;
        static const uint8_t commandReadEnhancedVolatileRegister;

        static const uint32_t sizeSector = 65536;
        static const uint32_t sizeSubSector = 4096;
        static const uint32_t sizePage = 256;

        static const uint8_t statusFlagWriteInProgress = 1;
        static const uint8_t volatileRegisterForQuadSpeed;

        FlashQuadSpi(hal::QuadSpi& spi, infra::Function<void()> onInitialized, uint32_t numberOfSubSectors = 4096);

    public:
        virtual void WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone) override;

    private:
        infra::BoundedVector<uint8_t>::WithMaxSize<4> ConvertAddress(uint32_t address) const;

        void WriteEnableSingleSpeed();
        void SwitchToQuadSpeed();
        void WriteEnable();
        void PageProgram();
        void EraseSomeSectors(uint32_t endIndex);
        void SendEraseSubSector(uint32_t subSectorIndex);
        void SendEraseSector(uint32_t subSectorIndex);
        void SendEraseBulk();
        void HoldWhileWriteInProgress();

    private:
        hal::QuadSpi& spi;
        infra::Function<void()> onInitialized;
        uint32_t numberOfSubSectors;
        infra::Sequencer sequencer;
        infra::TimerSingleShot initDelayTimer;
        infra::AutoResetFunction<void()> onDone;
        infra::ConstByteRange buffer;
        uint32_t address;
        uint32_t sectorIndex;
    };
}

#endif