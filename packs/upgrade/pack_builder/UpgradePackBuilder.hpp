#ifndef UPGRADE_PACK_BUILD_LIBRARY_UPGRADE_PACK_BUILDER_HPP
#define UPGRADE_PACK_BUILD_LIBRARY_UPGRADE_PACK_BUILDER_HPP

#include "hal/interfaces/FileSystem.hpp"
#include "infra/util/MemoryRange.hpp"
#include "packs/upgrade/pack/UpgradePackHeader.hpp"
#include "packs/upgrade/pack_builder/Input.hpp"
#include "packs/upgrade/pack_builder/InputFactory.hpp"
#include "packs/upgrade/pack_builder/ImageSigner.hpp"
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
        void AssignZeroFilled(const std::string& data, infra::MemoryRange<char> destination) const;
        void CheckSignature();

    private:
        HeaderInfo headerInfo;
        std::vector<std::unique_ptr<Input>> inputs;
        ImageSigner& signer;
        std::vector<uint8_t> upgradePack;
    };
}

#endif
