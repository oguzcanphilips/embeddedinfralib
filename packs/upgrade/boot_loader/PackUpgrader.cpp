#include "packs/upgrade/boot_loader/PackUpgrader.hpp"
#include <cstring>

namespace application
{
    PackUpgrader::PackUpgrader(hal::SynchronousFlash& upgradePackFlash)
        : upgradePackFlash(upgradePackFlash)
    {}

    void PackUpgrader::UpgradeFromImages(infra::MemoryRange<ImageUpgrader*> imageUpgraders)
    {
        UpgradePackHeaderPrologue headerPrologue;
        upgradePackFlash.ReadBuffer(infra::MakeByteRange(headerPrologue), address);
        address += sizeof(UpgradePackHeaderPrologue);

        bool sanity = (headerPrologue.status == UpgradePackStatus::readyToDeploy || headerPrologue.status == UpgradePackStatus::deployStarted) && headerPrologue.magic == upgradePackMagic;
        if (!sanity)
            return;

        MarkAsDeployStarted();

        address += headerPrologue.signatureLength;

        UpgradePackHeaderEpilogue headerEpilogue;
        upgradePackFlash.ReadBuffer(infra::MakeByteRange(headerEpilogue), address);
        address += sizeof(UpgradePackHeaderEpilogue);

        if (headerEpilogue.headerVersion != 1)
            MarkAsError(upgradeErrorCodeUnknownHeaderVersion);
        else
        {
            for (std::size_t imageIndex = 0; imageIndex != headerEpilogue.numberOfImages; ++imageIndex)
            {
                if (!TryUpgradeImage(imageUpgraders))
                    return;
            }

            MarkAsDeployed();
        }
    }

    bool PackUpgrader::TryUpgradeImage(infra::MemoryRange<ImageUpgrader*> imageUpgraders)
    {
        ImageHeaderPrologue imageHeader;
        upgradePackFlash.ReadBuffer(infra::MakeByteRange(imageHeader), address);

        uint32_t imageAddress = address;
        address += imageHeader.lengthOfHeaderAndImage;

        for (auto& imageUpgrader: imageUpgraders)
        {
            if (std::strncmp(imageHeader.targetName.data(), imageUpgrader->TargetName(), imageHeader.targetName.size()) == 0)
            {
                imageAddress += sizeof(imageHeader);

                infra::ByteRange decryptorState = imageUpgrader->ImageDecryptor().StateBuffer();
                upgradePackFlash.ReadBuffer(decryptorState, imageAddress);
                imageUpgrader->ImageDecryptor().Reset();
                imageAddress += decryptorState.size();

                ImageHeaderEpilogue imageHeaderEpilogue;
                upgradePackFlash.ReadBuffer(infra::MakeByteRange(imageHeaderEpilogue), imageAddress);
                imageAddress += sizeof(imageHeaderEpilogue);

                imageUpgrader->ImageDecryptor().DecryptPart(infra::MakeByteRange(imageHeaderEpilogue));
                uint32_t upgradeResult = imageUpgrader->Upgrade(upgradePackFlash, imageAddress, imageHeaderEpilogue.imageSize, imageHeaderEpilogue.destinationAddress);
                if (upgradeResult != 0)
                {
                    MarkAsError(upgradeResult);
                    return false;
                }

                return true;
            }
        }

        MarkAsError(upgradeErrorCodeNoSuitableImageUpgraderFound);
        return false;
    }

    void PackUpgrader::MarkAsDeployStarted()
    {
        static const UpgradePackStatus statusStarted = UpgradePackStatus::deployStarted;
        upgradePackFlash.WriteBuffer(infra::MakeByteRange(statusStarted), 0);
    }

    void PackUpgrader::MarkAsDeployed()
    {
        static const UpgradePackStatus statusDone = UpgradePackStatus::deployed;
        upgradePackFlash.WriteBuffer(infra::MakeByteRange(statusDone), 0);
    }

    void PackUpgrader::MarkAsError(uint32_t errorCode)
    {
        static const UpgradePackStatus statusError = UpgradePackStatus::invalid;
        upgradePackFlash.WriteBuffer(infra::MakeByteRange(statusError), 0);
        upgradePackFlash.WriteBuffer(infra::MakeByteRange(errorCode), 4);
    }
}
