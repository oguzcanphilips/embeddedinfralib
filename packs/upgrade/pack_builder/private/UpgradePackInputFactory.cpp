#include "packs/upgrade/pack_builder/public/InputBinary.hpp"
#include "packs/upgrade/pack_builder/public/InputHex.hpp"
#include "packs/upgrade/pack_builder/public/UpgradePackInputFactory.hpp"

namespace application
{
    SingleInputFactory::SingleInputFactory(const std::string& targetName)
        : targetName(targetName)
    {}

    std::string SingleInputFactory::TargetName() const
    {
        return targetName;
    }

    UpgradePackInputFactory::UpgradePackInputFactory(const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets,
        hal::FileSystem& fileSystem, const ImageSecurity& imageSecurity, const std::vector<SingleInputFactory*>& otherTargets)
        : supportedHexTargets(supportedHexTargets)
        , supportedBinaryTargets(supportedBinaryTargets)
        , fileSystem(fileSystem)
        , imageSecurity(imageSecurity)
        , otherTargets(otherTargets)
    {}

    std::unique_ptr<Input> UpgradePackInputFactory::CreateInput(const std::string& targetName, const std::string& fileName)
    {
        if (std::find(supportedHexTargets.begin(), supportedHexTargets.end(), targetName) != supportedHexTargets.end())
            return std::make_unique<InputHex>(targetName, fileName, fileSystem, imageSecurity);

        for (auto targetAndAddress : supportedBinaryTargets)
            if (targetAndAddress.first == targetName)
                return std::make_unique<InputBinary>(targetName, fileName, targetAndAddress.second, fileSystem, imageSecurity);

        for (auto target : otherTargets)
            if (target->TargetName() == targetName)
                return target->CreateInput();

        throw std::exception((std::string("Unknown target: ") + targetName).c_str());
    }
}
