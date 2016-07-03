#ifndef UPGRADE_PACK_BUILD_LIBRARY_UPGRADE_PACK_BUILDER_HPP
#define UPGRADE_PACK_BUILD_LIBRARY_UPGRADE_PACK_BUILDER_HPP

#include "hal/interfaces/public/FileSystem.hpp"
#include "upgrade_pack/UpgradePackHeader.hpp"
#include "upgrade_pack_builder_library/Input.hpp"
#include "upgrade_pack_builder_library/InputFactory.hpp"
#include "upgrade_pack_builder_library/ImageSigner.hpp"
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
