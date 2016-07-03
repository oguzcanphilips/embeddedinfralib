#ifndef UPGRADE_PACK_UPGRADER_HPP
#define UPGRADE_PACK_UPGRADER_HPP

#include "upgrade_boot_loader/ImageUpgrader.hpp"
#include "infra/util/public/ByteRange.hpp"
#include "hal/synchronous_interfaces/public/SynchronousFlash.hpp"
#include "upgrade_pack/UpgradePackHeader.hpp"

namespace application
{
    class PackUpgrader
    {
    public:
        PackUpgrader(hal::SynchronousFlash& upgradePackFlash);

        void UpgradeFromImages(infra::MemoryRange<ImageUpgrader*> imageUpgraders);

    private:
        bool TryUpgradeImage(infra::MemoryRange<ImageUpgrader*> imageUpgraders);
        void MarkAsDeployed();
        void MarkAsError(uint32_t errorCode);

    private:
        hal::SynchronousFlash& upgradePackFlash;
        uint32_t address = 0;
    };
}

#endif
