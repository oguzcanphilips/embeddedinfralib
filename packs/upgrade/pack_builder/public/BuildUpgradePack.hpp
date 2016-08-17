#ifndef UPGRADE_PACK_BUILD_LIBRARY_BUILD_UPGRADE_PACK_HPP
#define UPGRADE_PACK_BUILD_LIBRARY_BUILD_UPGRADE_PACK_HPP

#include "infra/util/public/ByteRange.hpp"
#include "packs/upgrade/pack_builder/public/UpgradePackBuilder.hpp"
#include <string>
#include <vector>
#include <utility>

namespace application
{
    int BuildUpgradePack(const application::UpgradePackBuilder::HeaderInfo& headerInfo, const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, const char* argv[], infra::ConstByteRange aesKey, infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey);

    class UpgradePackBuilderFacade
    {
    public:
        UpgradePackBuilderFacade(const application::UpgradePackBuilder::HeaderInfo& headerInfo);
            
        void Build(const std::vector<std::string>& supportedHexTargets, const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets,
            int argc, const char* argv[], infra::ConstByteRange aesKey, infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey);

        int Result() const;

    protected:
        virtual void ParseArgument(int& index, int argc, const char* argv[]);
        virtual void PreBuilder();
        virtual void PostBuilder(UpgradePackBuilder& builder, ImageSigner& signer);

    protected:
        UpgradePackBuilder::HeaderInfo headerInfo;

    private:
        int result = 0;
        std::vector<std::pair<std::string, std::string>> targetAndFiles;
    };
}

#endif
