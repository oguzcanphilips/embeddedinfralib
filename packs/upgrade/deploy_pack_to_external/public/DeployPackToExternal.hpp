#ifndef UPGRADE_DEPLOY_PACK_TO_EXTERNAL_HPP
#define UPGRADE_DEPLOY_PACK_TO_EXTERNAL_HPP

#include "hal/interfaces/public/Flash.hpp"
#include "hal/interfaces/public/Gpio.hpp"
#include "infra/util/public/ByteRange.hpp"
#include "infra/util/public/Observer.hpp"
#include "infra/util/public/Sequencer.hpp"
#include "packs/upgrade/pack/public/UpgradePackHeader.hpp"

namespace application
{
    class DeployPackToExternal;

    class DeployPackToExternalObserver
        : public infra::SingleObserver<DeployPackToExternalObserver, DeployPackToExternal>
    {
    public:
        using infra::SingleObserver<DeployPackToExternalObserver, DeployPackToExternal>::SingleObserver;

        virtual void NotDeployable() = 0;
        virtual void DoesntFit() = 0;
        virtual void Done() = 0;
    };

    class DeployPackToExternal
        : public infra::Subject<DeployPackToExternalObserver>
    {
    public:
        DeployPackToExternal(hal::Flash& from, hal::Flash& to);

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
    };
}

#endif
