#ifndef UPGRADE_PACK_BUILDER_LIBRARY_UPGRADE_PACK_INPUT_FACTORY_HPP
#define UPGRADE_PACK_BUILDER_LIBRARY_UPGRADE_PACK_INPUT_FACTORY_HPP

#include "packs/upgrade/pack_builder/public/InputFactory.hpp"
#include "packs/upgrade/pack_builder/public/ImageSecurity.hpp"

namespace application
{
    class SingleInputFactory
    {
    public:
        SingleInputFactory(const std::string& targetName);
        SingleInputFactory(const SingleInputFactory& other) = delete;
        SingleInputFactory& operator=(const SingleInputFactory& other) = delete;
        virtual ~SingleInputFactory() = default;

        virtual std::unique_ptr<Input> CreateInput() const = 0;

        std::string TargetName() const;

    private:
        std::string targetName;
    };

    class UpgradePackInputFactory
        : public InputFactory
    {
    public:
        UpgradePackInputFactory(const std::vector<std::string>& supportedHexTargets,
            const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets,
            hal::FileSystem& fileSystem, const ImageSecurity& imageSecurity, const std::vector<SingleInputFactory*>& otherTargets);

        virtual std::unique_ptr<Input> CreateInput(const std::string& targetName, const std::string& fileName) override;

    private:
        std::vector<std::string> supportedHexTargets;
        std::vector<std::pair<std::string, uint32_t>> supportedBinaryTargets;
        hal::FileSystem& fileSystem;
        const ImageSecurity& imageSecurity;
        const std::vector<SingleInputFactory*>& otherTargets;
    };
}

#endif
