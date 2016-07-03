#ifndef UPGRADE_PACK_BUILDER_LIBRARY_UPGRADE_PACK_INPUT_FACTORY_HPP
#define UPGRADE_PACK_BUILDER_LIBRARY_UPGRADE_PACK_INPUT_FACTORY_HPP

#include "upgrade_pack_builder_library/InputFactory.hpp"
#include "upgrade_pack_builder_library/ImageSecurity.hpp"

namespace application
{
    class UpgradePackInputFactory
        : public InputFactory
    {
    public:
        UpgradePackInputFactory(const std::vector<std::string>& supportedHexTargets,
            const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets,
            hal::FileSystem& fileSystem, const ImageSecurity& imageSecurity);

        virtual std::unique_ptr<Input> CreateInput(const std::string& targetName, const std::string& fileName) override;

    private:
        std::vector<std::string> supportedHexTargets;
        std::vector<std::pair<std::string, uint32_t>> supportedBinaryTargets;
        hal::FileSystem& fileSystem;
        const ImageSecurity& imageSecurity;
    };
}

#endif
