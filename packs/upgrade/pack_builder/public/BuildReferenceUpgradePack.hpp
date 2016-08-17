#ifndef UPGRADE_BUILD_REFERENCE_UPGRADE_PACK_HPP
#define UPGRADE_BUILD_REFERENCE_UPGRADE_PACK_HPP

#include "infra/util/public/ByteRange.hpp"
#include "packs/upgrade/pack_builder/public/BuildUpgradePack.hpp"
#include <string>
#include <vector>
#include <utility>

namespace application
{
    int BuildReferenceUpgradePack(const application::UpgradePackBuilder::HeaderInfo& headerInfo, const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, const char* argv[], infra::ConstByteRange aesKey, infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey);

    class ReferenceUpgradePackBuilderFacade
        : public UpgradePackBuilderFacade
    {
    public:
        ReferenceUpgradePackBuilderFacade(const application::UpgradePackBuilder::HeaderInfo& headerInfo, const std::vector<std::string>& supportedHexTargets,
            const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, const char* argv[], infra::ConstByteRange aesKey, infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey);
    
    private:
        virtual void ParseArgument(int& index, int argc, const char* argv[]) override;
        virtual void PreBuilder() override;
        virtual void PostBuilder(UpgradePackBuilder& builder, ImageSigner& signer) override;

    private:
        bool invalidHeaderVersion = false;
        bool invalidProduct = false;
        bool invalidSignature = false;
    };
}

#endif
