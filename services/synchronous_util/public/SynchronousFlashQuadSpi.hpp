#ifndef SERVICES_SYNCHRONOUS_FLASH_QUAD_SPI_HPP
#define SERVICES_SYNCHRONOUS_FLASH_QUAD_SPI_HPP

#include "hal/synchronous_interfaces/public/SynchronousFlashHomogeneous.hpp"
#include "hal/synchronous_interfaces/public/SynchronousQuadSpi.hpp"

namespace services
{
    class SynchronousFlashQuadSpi
        : public hal::SynchronousFlashHomogeneous
    {
    public:
        static const uint32_t sizeSector = 65536;
        static const uint32_t sizeSubSector = 4096;
        static const uint32_t sizePage = 256;

        SynchronousFlashQuadSpi(hal::SynchronousQuadSpi& quadSpi);

        virtual void WriteBuffer(infra::ConstByteRange buffer, uint32_t address) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint32_t address) override;
        virtual void EraseSectors(uint32_t beginIndex, uint32_t endIndex) override;

    private:
        void WriteEnable();
        void PageProgram();
        void HoldWhileWriteInProgress();
        uint8_t ReadStatusRegister();

    private:
        hal::SynchronousQuadSpi& quadSpi;
        infra::ConstByteRange buffer;
        uint32_t address;
    };
}

#endif
