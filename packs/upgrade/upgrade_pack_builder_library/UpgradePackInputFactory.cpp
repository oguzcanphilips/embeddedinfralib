#include "upgrade_pack_builder_library/InputBinary.hpp"
#include "upgrade_pack_builder_library/InputHex.hpp"
#include "upgrade_pack_builder_library/UpgradePackInputFactory.hpp"

namespace application
{
    UpgradePackInputFactory::UpgradePackInputFactory(const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets,
        hal::FileSystem& fileSystem, const ImageSecurity& imageSecurity)
        : supportedHexTargets(supportedHexTargets)
        , supportedBinaryTargets(supportedBinaryTargets)
        , fileSystem(fileSystem)
        , imageSecurity(imageSecurity)
    {}

    std::unique_ptr<Input> UpgradePackInputFactory::CreateInput(const std::string& targetName, const std::string& fileName)
    {
        if (std::find(supportedHexTargets.begin(), supportedHexTargets.end(), targetName) != supportedHexTargets.end())
            return std::make_unique<InputHex>(targetName, fileName, fileSystem, imageSecurity);

        for (auto targetAndAddress : supportedBinaryTargets)
            if (targetAndAddress.first == targetName)
                return std::make_unique<InputBinary>(targetName, fileName, targetAndAddress.second, fileSystem, imageSecurity);

        throw std::exception((std::string("Unknown target: ") + targetName).c_str());
    }
}
