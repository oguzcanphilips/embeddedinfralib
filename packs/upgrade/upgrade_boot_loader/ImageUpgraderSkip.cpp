#include "upgrade_boot_loader/ImageUpgraderSkip.hpp"

namespace application
{
    ImageUpgraderSkip::ImageUpgraderSkip(const char* targetName)
        : ImageUpgrader(targetName, decryptorNone)
    {}

    bool ImageUpgraderSkip::Upgrade(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t destinationAddress)
    {
        return true;
    }
}
