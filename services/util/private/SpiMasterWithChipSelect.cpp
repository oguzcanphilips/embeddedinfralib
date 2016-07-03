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

    uint32_t SpiMasterWithChipSelect::Speed() const
    {
        return spi.Speed();
    }

    void SpiMasterWithChipSelect::ConfigSpeed(uint32_t speedInkHz)
    {
        spi.ConfigSpeed(speedInkHz);
    }

    uint8_t SpiMasterWithChipSelect::Mode() const
    {
        return spi.Mode();
    }

    void SpiMasterWithChipSelect::ConfigMode(uint8_t spiMode)
    {
        spi.ConfigMode(spiMode);
    }
}
