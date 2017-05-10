#ifndef UPGRADE_PACK_UPGRADER_HPP
#define UPGRADE_PACK_UPGRADER_HPP

#include "packs/upgrade/boot_loader/public/ImageUpgrader.hpp"
#include "infra/util/public/ByteRange.hpp"
#include "hal/synchronous_interfaces/public/SynchronousFlash.hpp"
#include "packs/upgrade/pack/public/UpgradePackHeader.hpp"

namespace application
{
    class PackUpgrader
    {
    public:
        explicit PackUpgrader(hal::SynchronousFlash& upgradePackFlash);

        void UpgradeFromImages(infra::MemoryRange<ImageUpgrader*> imageUpgraders);

    private:
        bool TryUpgradeImage(infra::MemoryRange<ImageUpgrader*> imageUpgraders);
        void MarkAsDeployStarted();
        void MarkAsDeployed();
        void MarkAsError(uint32_t errorCode);

    private:
        hal::SynchronousFlash& upgradePackFlash;
        uint32_t address = 0;
    };
}

#endif
