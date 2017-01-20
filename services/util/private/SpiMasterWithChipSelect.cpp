#include "services/util/public/SpiMasterWithChipSelect.hpp"

namespace services
{
    SpiMasterWithChipSelect::SpiMasterWithChipSelect(hal::SpiMaster& spi, hal::GpioPin& chipSelect)
        : spi(spi)
        , chipSelect(chipSelect, true)
    {}

    void SpiMasterWithChipSelect::SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart)
    {
        onDone = actionOnCompletion;
        onStart = actionOnStart;

        spi.SendAndReceive(sendData, receiveData, nextAction, [this, nextAction]()
        {
            if (nextAction == hal::SpiAction::stop)
                chipSelect.Set(true);
            onDone();
        }, [this]()
        {
            chipSelect.Set(false);
            onStart();
        });
    }

    void SpiMasterWithChipSelect::SetCommunicationConfigurator(hal::CommunicationConfigurator& configurator)
    {
        spi.SetCommunicationConfigurator(configurator);
    }

    void SpiMasterWithChipSelect::ResetCommunicationConfigurator()
    {
        spi.ResetCommunicationConfigurator();
    }
}
