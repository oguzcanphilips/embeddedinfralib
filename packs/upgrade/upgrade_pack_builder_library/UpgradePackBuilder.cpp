#include "UpgradePackBuilder.hpp"
#include "ImageSignerRsa.hpp"
#include <algorithm>
#include <cassert>

namespace application
{
    UpgradePackBuilder::UpgradePackBuilder(const std::vector<std::pair<std::string, std::string>>& targetAndFiles,
        const HeaderInfo& headerInfo, InputFactory& inputFactory, ImageSigner& imageSigner)
        : headerInfo(headerInfo)
        , signer(imageSigner)
    {
        for (auto targetAndFile : targetAndFiles)
            inputs.push_back(inputFactory.CreateInput(targetAndFile.first, targetAndFile.second));

        CreateUpgradePack();
    }

    UpgradePackBuilder::UpgradePackBuilder(std::vector<std::unique_ptr<Input>>&& inputs, ImageSigner& signer)
        : inputs(std::move(inputs))
        , signer(signer)
    {
        CreateUpgradePack();
    }

    std::vector<uint8_t>& UpgradePackBuilder::UpgradePack()
    {
        return upgradePack;
    }

    void UpgradePackBuilder::WriteUpgradePack(const std::string& fileName, hal::FileSystem& fileSystem)
    {
        assert(!upgradePack.empty());

        fileSystem.WriteBinaryFile(fileName, upgradePack);
    }

    void UpgradePackBuilder::CreateUpgradePack()
    {
        AddEpilogue();
        AddImages();
        AddPrologueAndSignature();

        CheckSignature();
    }

    void UpgradePackBuilder::AddPrologueAndSignature()
    {
        std::vector<uint8_t> signature = signer.ImageSignature(upgradePack);

        UpgradePackHeaderPrologue prologue = {};
        prologue.status = UpgradePackStatus::readyToDeploy;
        prologue.magic = upgradePackMagic;
        prologue.errorCode = 0xffffffff;
        prologue.signedContentsLength = upgradePack.size();
        prologue.signatureMethod = signer.SignatureMethod();
        prologue.signatureLength = signature.size();

        upgradePack.insert(upgradePack.begin(), signature.begin(), signature.end());
        upgradePack.insert(upgradePack.begin(), reinterpret_cast<const uint8_t*>(&prologue), reinterpret_cast<const uint8_t*>(&prologue + 1));
    }

    void UpgradePackBuilder::AddEpilogue()
    {
        UpgradePackHeaderEpilogue epilogue = {};
        epilogue.headerVersion = 1;
        epilogue.headerLength = sizeof(UpgradePackHeaderPrologue) + signer.SignatureLength() + sizeof(UpgradePackHeaderEpilogue);
        epilogue.numberOfImages = inputs.size();

        AssignZeroFilled(headerInfo.productName, epilogue.productName, sizeof(epilogue.productName));
        AssignZeroFilled(headerInfo.productVersion, epilogue.productVersion, sizeof(epilogue.productName));
        AssignZeroFilled(headerInfo.componentName, epilogue.componentName, sizeof(epilogue.productName));
        epilogue.componentVersion = headerInfo.componentVersion;

        upgradePack.assign(reinterpret_cast<const uint8_t*>(&epilogue), reinterpret_cast<const uint8_t*>(&epilogue + 1));
    }

    void UpgradePackBuilder::AddImages()
    {
        for (const std::unique_ptr<Input>& input : inputs)
        {
            std::vector<uint8_t> image = input->Image();
            upgradePack.insert(upgradePack.end(), image.begin(), image.end());
        }
    }

    void UpgradePackBuilder::AssignZeroFilled(const std::string& data, char* destination, std::size_t size) const
    {
        std::copy(data.begin(), data.begin() + std::min(data.size(), size), destination);
    }

    void UpgradePackBuilder::CheckSignature()
    {
        std::vector<uint8_t> signature(upgradePack.begin() + sizeof(UpgradePackHeaderPrologue), upgradePack.begin() + sizeof(UpgradePackHeaderPrologue) + signer.SignatureLength());

        if (!signer.CheckSignature(signature, std::vector<uint8_t>(upgradePack.begin() + sizeof(UpgradePackHeaderPrologue) + signer.SignatureLength(), upgradePack.end())))
            throw SignatureDoesNotVerifyException();
    }
}
