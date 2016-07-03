#ifndef UPGRADE_IMAGE_UPGRADER_HPP
#define UPGRADE_IMAGE_UPGRADER_HPP

#include "upgrade_boot_loader/Decryptor.hpp"
#include "hal/synchronous_interfaces/public/SynchronousFlash.hpp"

namespace application
{
    class ImageUpgrader
    {
    public:
        ImageUpgrader(const char* targetName, Decryptor& decryptor);
        ImageUpgrader(const ImageUpgrader& other) = delete;
        ImageUpgrader& operator=(const ImageUpgrader& other) = delete;

        const char* TargetName() const;
        Decryptor& ImageDecryptor();
        virtual bool Upgrade(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t destinationAddress) = 0;

    protected:
        ~ImageUpgrader() = default;

    private:
        const char* targetName;
        Decryptor& decryptor;
    };
}

#endif
