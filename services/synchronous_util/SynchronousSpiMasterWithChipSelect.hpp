#ifndef SERVICES_SYNCHRONOUS_SPI_MASTER_WITH_CHIP_SELECT_HPP
#define SERVICES_SYNCHRONOUS_SPI_MASTER_WITH_CHIP_SELECT_HPP

#include "hal/synchronous_interfaces/SynchronousSpi.hpp"
#include "hal/synchronous_interfaces/SynchronousGpio.hpp"

namespace services
{
    class SynchronousSpiMasterWithChipSelect
        : public hal::SynchronousSpi
    {
    public:
        SynchronousSpiMasterWithChipSelect(hal::SynchronousSpi& spi, hal::SynchronousOutputPin& chipSelect);

        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, Action nextAction) override;

    private:
        hal::SynchronousSpi& spi;
        hal::SynchronousOutputPin& chipSelect;
    };
}

#endif
