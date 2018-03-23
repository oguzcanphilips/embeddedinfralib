#include "hal/windows/FileSystemWin.hpp"
#include "infra/syntax/Json.hpp"
#include "upgrade/pack_builder/BuildReferenceUpgradePack.hpp"
#include "upgrade/pack_builder/UpgradePackBuilder.hpp"
#include "upgrade/pack_builder/UpgradePackInputFactory.hpp"
#include <cctype>

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
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, const char* argv[], infra::ConstByteRange aesKey,
        infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey, const std::vector<NoFileInputFactory*>& otherTargets)
    {
        ReferenceUpgradePackBuilderFacade builderFacade(headerInfo);
        builderFacade.Build(supportedHexTargets, supportedBinaryTargets, argc, argv, aesKey, ecDsa224PublicKey, ecDsa224PrivateKey, otherTargets);
        return builderFacade.Result();
    }

    int BuildReferenceUpgradePack(const application::UpgradePackBuilder::HeaderInfo& headerInfo, const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, std::string outputFilename,
        std::vector<std::pair<std::string, std::string>>& targetAndFiles, std::vector<std::pair<std::string, std::string>>& buildOptions, infra::ConstByteRange aesKey,
        infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey, const std::vector<NoFileInputFactory*>& otherTargets)
    {
        ReferenceUpgradePackBuilderFacade builderFacade(headerInfo);
        builderFacade.Build(supportedHexTargets, supportedBinaryTargets, outputFilename, targetAndFiles, buildOptions, aesKey, ecDsa224PublicKey, ecDsa224PrivateKey, otherTargets);
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

    void ReferenceUpgradePackBuilderFacade::PreBuilder(std::vector<std::pair<std::string, std::string>> buildOptions)
    {
        for (auto option : buildOptions)
            if (option.first == "invalidProduct")
                invalidProduct = true;

        PreBuilder();
    }

    void ReferenceUpgradePackBuilderFacade::PostBuilder(UpgradePackBuilder& builder, ImageSigner& signer, std::vector<std::pair<std::string, std::string>> buildOptions)
    {
        for (auto option : buildOptions)
            if (option.first == "invalidHeaderVersion")
                invalidHeaderVersion = true;
            else if (option.first == "invalidSignature")
                invalidSignature = true;

        PostBuilder(builder, signer);
    }
}
