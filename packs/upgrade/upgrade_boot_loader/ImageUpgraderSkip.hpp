#ifndef UPGRADE_IMAGE_UPGRADER_SKIP_HPP
#define UPGRADE_IMAGE_UPGRADER_SKIP_HPP

#include "upgrade_boot_loader/DecryptorNone.hpp"
#include "upgrade_boot_loader/ImageUpgrader.hpp"

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
