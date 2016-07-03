#include "upgrade_boot_loader/ImageUpgraderInternalFlash.hpp"

namespace application
{
    ImageUpgraderInternalFlash::ImageUpgraderInternalFlash(infra::ByteRange buffer, const char* targetName, Decryptor& decryptor, hal::SynchronousFlash& internalFlash, uint32_t destinationAddressOffset)
        : buffer(buffer)
        , ImageUpgrader(targetName, decryptor)
        , internalFlash(&internalFlash)
        , destinationAddressOffset(destinationAddressOffset)
    {}

    bool ImageUpgraderInternalFlash::Upgrade(hal::SynchronousFlash& upgradePackFlash, uint32_t imageAddress, uint32_t imageSize, uint32_t destinationAddress)
    {
        destinationAddress += destinationAddressOffset;

        internalFlash->EraseSectors(internalFlash->SectorOfAddress(destinationAddress), internalFlash->SectorOfAddress(destinationAddress + imageSize - 1) + 1);

        uint32_t imageAddressStart = imageAddress;
        while (imageAddress - imageAddressStart < imageSize)
        {
            infra::ByteRange bufferRange(buffer);
            bufferRange.shrink_from_back_to(imageSize - (imageAddress - imageAddressStart));
            upgradePackFlash.ReadBuffer(bufferRange, imageAddress);
            imageAddress += bufferRange.size();

            ImageDecryptor().DecryptPart(bufferRange);

            internalFlash->WriteBuffer(bufferRange, destinationAddress);
            destinationAddress += bufferRange.size();
        }

        return true;
    }

    void ImageUpgraderInternalFlash::SetFlash(hal::SynchronousFlash& flash)
    {
        internalFlash = &flash;
    }
}
