#include "hal/windows/public/FileSystemWin.hpp"
#include "packs/upgrade/pack_builder/public/BinaryObject.hpp"
#include "mbedtls/memory_buffer_alloc.h"
#include "packs/upgrade/pack_builder/public/BuildReferenceUpgradePack.hpp"
#include "packs/upgrade/pack_builder/public/ImageEncryptorAes.hpp"
#include "packs/upgrade/pack_builder/public/ImageSignerEcDsa.hpp"
#include "packs/upgrade/pack_builder/public/UpgradePackBuilder.hpp"
#include "packs/upgrade/pack_builder/public/UpgradePackInputFactory.hpp"
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <windows.h>

namespace application
{
    namespace
    {
        std::string ToLower(const std::string& str)
        {
            std::string result;
            std::transform(str.begin(), str.end(), std::back_inserter(result), std::tolower);
            return result;
        }

        struct UsageException
        {};
    }

    int BuildReferenceUpgradePack(const application::UpgradePackBuilder::HeaderInfo& headerInfo, const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, const char* argv[], infra::ConstByteRange aesKey, infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey)
    {
        ReferenceUpgradePackBuilderFacade builderFacade(headerInfo);
        builderFacade.Build(supportedHexTargets, supportedBinaryTargets, argc, argv, aesKey, ecDsa224PublicKey, ecDsa224PrivateKey);
        return builderFacade.Result();
    }

    ReferenceUpgradePackBuilderFacade::ReferenceUpgradePackBuilderFacade(const application::UpgradePackBuilder::HeaderInfo& headerInfo)
        : UpgradePackBuilderFacade(headerInfo)
    {}

    void ReferenceUpgradePackBuilderFacade::ParseArgument(int& index, int argc, const char* argv[])
    {
        if (argv[index] == std::string("-invalid_header_version"))
            invalidHeaderVersion = true;
        else if (argv[index] == std::string("-invalid_product"))
            invalidProduct = true;
        else if (argv[index] == std::string("-invalid_signature"))
            invalidSignature = true;
        else
            UpgradePackBuilderFacade::ParseArgument(index, argc, argv);
    }

    void ReferenceUpgradePackBuilderFacade::PreBuilder()
    {
        if (invalidProduct)
            headerInfo.productName = "Unknown Product Name";
    }

    void ReferenceUpgradePackBuilderFacade::PostBuilder(UpgradePackBuilder& builder, ImageSigner& signer)
    {
        if (invalidHeaderVersion)
            reinterpret_cast<UpgradePackHeaderEpilogue*>(builder.UpgradePack().data() + sizeof(UpgradePackHeaderPrologue) + signer.SignatureLength())->headerVersion = 0xff;
        if (invalidSignature)
            std::fill(builder.UpgradePack().begin() + sizeof(UpgradePackHeaderPrologue), builder.UpgradePack().begin() + sizeof(UpgradePackHeaderPrologue) + signer.SignatureLength(), 0xff);
    }
}
