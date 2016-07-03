#ifndef UPGRADE_DEPLOY_PACK_TO_EXTERNAL_HPP
#define UPGRADE_DEPLOY_PACK_TO_EXTERNAL_HPP

#include "hal/interfaces/public/Flash.hpp"
#include "hal/interfaces/public/Gpio.hpp"
#include "infra/util/public/ByteRange.hpp"
#include "infra/util/public/Sequencer.hpp"
#include "services/util/public/SignalLed.hpp"
#include "upgrade_pack/UpgradePackHeader.hpp"

namespace application
{
    class DeployPackToExternal
    {
    public:
        DeployPackToExternal(hal::Flash& from, hal::Flash& to, hal::GpioPin& statusLedPin);

    private:
        hal::Flash& from;
        hal::Flash& to;

        infra::Sequencer sequencer;

        UpgradePackHeaderPrologue header;
        std::array<uint8_t, 256> buffer;
        std::size_t sizeToDo = 0;
        std::size_t currentSize = 0;
        uint32_t currentReadAddress = 0;
        uint32_t currentWriteAddress = 0;
        services::SignalLed statusLed;
    };
}

#endif
