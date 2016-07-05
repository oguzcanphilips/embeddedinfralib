#ifndef UPGRADE_PACK_BUILD_LIBRARY_UPGRADE_PACK_BUILDER_HPP
#define UPGRADE_PACK_BUILD_LIBRARY_UPGRADE_PACK_BUILDER_HPP

#include "hal/interfaces/public/FileSystem.hpp"
#include "packs/upgrade/pack/public/UpgradePackHeader.hpp"
#include "packs/upgrade/pack_builder/public/Input.hpp"
#include "packs/upgrade/pack_builder/public/InputFactory.hpp"
#include "packs/upgrade/pack_builder/public/ImageSigner.hpp"
#include <memory>
#include <vector>

namespace application
{
    class SignatureDoesNotVerifyException
    {};

    class UpgradePackBuilder
    {
    public:
        struct HeaderInfo
        {
            std::string productName;
            std::string productVersion;
            std::string componentName;
            uint32_t componentVersion;
        };

    public:
        UpgradePackBuilder(const std::vector<std::pair<std::string, std::string>>& targetAndFiles, const HeaderInfo& headerInfo,
            InputFactory& inputFactory, ImageSigner& imageSigner);
        UpgradePackBuilder(std::vector<std::unique_ptr<Input>>&& inputs, ImageSigner& signer);

        std::vector<uint8_t>& UpgradePack();
        void WriteUpgradePack(const std::string& fileName, hal::FileSystem& fileSystem);

    private:
        void CreateUpgradePack();
        void AddPrologueAndSignature();
        void AddEpilogue();
        void AddImages();
        void AssignZeroFilled(const std::string& data, char* destination, std::size_t size) const;
        void CheckSignature();

    private:
        HeaderInfo headerInfo;
        std::vector<std::unique_ptr<Input>> inputs;
        ImageSigner& signer;
        std::vector<uint8_t> upgradePack;
    };
}

#endif
