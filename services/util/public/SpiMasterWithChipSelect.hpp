#ifndef SERVICES_SPI_MASTER_WITH_CHIP_SELECT_HPP
#define SERVICES_SPI_MASTER_WITH_CHIP_SELECT_HPP

#include "hal/interfaces/public/Gpio.hpp"
#include "hal/interfaces/public/Spi.hpp"

namespace services
{
    class SpiMasterWithChipSelect
        : public hal::SpiMaster
    {
    public:
        SpiMasterWithChipSelect(hal::SpiMaster& aSpi, hal::GpioPin& aChipSelect);

    public:
        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart = infra::emptyFunction) override;
        virtual void SetCommunicationConfigurator(hal::CommunicationConfigurator& configurator) override;
        virtual void ResetCommunicationConfigurator() override;

    private:
        hal::SpiMaster& spi;
        hal::OutputPin chipSelect;
        infra::Function<void()> onStart;
        infra::Function<void()> onDone;
    };
}

#endif
