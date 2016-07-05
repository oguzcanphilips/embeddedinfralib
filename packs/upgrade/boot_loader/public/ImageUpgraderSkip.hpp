#ifndef UPGRADE_IMAGE_UPGRADER_SKIP_HPP
#define UPGRADE_IMAGE_UPGRADER_SKIP_HPP

#include "packs/upgrade/boot_loader/public/DecryptorNone.hpp"
#include "packs/upgrade/boot_loader/public/ImageUpgrader.hpp"

namespace application
{
    class ImageUpgraderSkip
        : public ImageUpgrader
    {
    public:
        ImageUpgraderSkip(const char* targetName);

        virtual bool Upgrade(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t destinationAddress) override;

    private:
        DecryptorNone decryptorNone;
    };
}

#endif
