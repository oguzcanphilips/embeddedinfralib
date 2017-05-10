#include "mbedtls/memory_buffer_alloc.h"
#include "hal/windows/public/FileSystemWin.hpp"
#include "packs/upgrade/pack_builder/public/BinaryObject.hpp"
#include "packs/upgrade/pack_builder/public/BuildUpgradePack.hpp"
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

    int BuildUpgradePack(const application::UpgradePackBuilder::HeaderInfo& headerInfo, const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, const char* argv[], infra::ConstByteRange aesKey,
        infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey, const std::vector<NoFileInputFactory*>& otherTargets)
    {
        UpgradePackBuilderFacade builderFacade(headerInfo);
        builderFacade.Build(supportedHexTargets, supportedBinaryTargets, argc, argv, aesKey, ecDsa224PublicKey, ecDsa224PrivateKey, otherTargets);
        return builderFacade.Result();
    }

    UpgradePackBuilderFacade::UpgradePackBuilderFacade(const application::UpgradePackBuilder::HeaderInfo& headerInfo)
        : headerInfo(headerInfo)
    {
        // Hopefully unneeded, but just in case anyone uses the rand() function seed it with something quasi-random
        std::srand(static_cast<unsigned int>(std::time(nullptr)) * static_cast<unsigned int>(GetCurrentProcessId()));

        // Initialize the MbedTLS memory pool
        unsigned char memory_buf[100000];
        mbedtls_memory_buffer_alloc_init(memory_buf, sizeof(memory_buf));
    }

    void UpgradePackBuilderFacade::Build(const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, const char* argv[], infra::ConstByteRange aesKey,
        infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey, const std::vector<NoFileInputFactory*>& otherTargets)
    {
        try
        {
            TryBuild(supportedHexTargets, supportedBinaryTargets, argc, argv, aesKey, ecDsa224PublicKey, ecDsa224PrivateKey, otherTargets);
        }
        catch (UsageException&)
        {
            std::cout << "Arugments: ";
            for (int i = 0; i != argc; ++i)
                std::cout << " " << argv[i];
            std::cout << std::endl;
            std::cout << "Invalid number of arguments" << std::endl;
            std::cout << argv[0] << " OutputFile [-Target1 InputFile1] [-Target2 InputFile2] [-Target3 InputFile3] [-Target4 InputFile4] ..." << std::endl;
            std::cout << "Targets: ";

            for (auto target : supportedHexTargets)
                std::cout << target << " ";
            for (auto targetAndAddress : supportedBinaryTargets)
                std::cout << targetAndAddress.first << " ";
            for (auto target : otherTargets)
                std::cout << target->TargetName() << " ";
            std::cout << std::endl;

            result = 1;
        }
        catch (application::IncorrectCrcException& exception)
        {
            std::cout << "Incorrect CRC in file " << exception.file << " at line " << exception.line << std::endl;
            result = 1;
        }
        catch (application::NoEndOfFileException& exception)
        {
            std::cout << "No end of file found in file " << exception.file << " at line " << exception.line << std::endl;
            result = 1;
        }
        catch (application::DataAfterEndOfFileException& exception)
        {
            std::cout << "Data found after end of file in file " << exception.file << " at line " << exception.line << std::endl;
            result = 1;
        }
        catch (application::UnknownRecordException& exception)
        {
            std::cout << "Unknown record in file " << exception.file << " at line " << exception.line << std::endl;
            result = 1;
        }
        catch (application::RecordTooShortException& exception)
        {
            std::cout << "Record too short in file " << exception.file << " at line " << exception.line << std::endl;
            result = 1;
        }
        catch (application::RecordTooLongException& exception)
        {
            std::cout << "Record too long in file " << exception.file << " at line " << exception.line << std::endl;
            result = 1;
        }
        catch (application::OverwriteException& exception)
        {
            std::cout << "Contents specified twice for memory location at address 0x" << std::hex << std::setw(8) << std::setfill('0') << exception.position << std::endl;
            result = 1;
        }
        catch (hal::CannotOpenFileException& exception)
        {
            std::cout << "Cannot open file " << exception.name << std::endl;
            result = 1;
        }
        catch (application::SignatureDoesNotVerifyException&)
        {
            std::cout << "Signature does not verify" << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
            result = 1;
        }
    }

    void UpgradePackBuilderFacade::TryBuild(const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, const char* argv[], infra::ConstByteRange aesKey,
        infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey, const std::vector<NoFileInputFactory*>& otherTargets)
    {
        if (argc < 3)
            throw UsageException();

        std::string outputFilename = argv[1];
        std::cout << "Creating upgrade pack " << outputFilename << "..." << std::endl;

        for (int i = 2; i < argc; ++i)
            ParseArgument(i, argc, argv);

        application::SecureRandomNumberGenerator randomNumberGenerator;
        hal::FileSystemWin fileSystem;
        application::ImageEncryptorAes imageEncryptorAes(randomNumberGenerator, aesKey);
        application::UpgradePackInputFactory inputFactory(supportedHexTargets, supportedBinaryTargets, fileSystem, imageEncryptorAes, otherTargets);
        application::ImageSignerEcDsa signer(randomNumberGenerator, ecDsa224PublicKey, ecDsa224PrivateKey);

        PreBuilder();
        application::UpgradePackBuilder builder(targetAndFiles, this->headerInfo, inputFactory, signer);
        PostBuilder(builder, signer);

        builder.WriteUpgradePack(outputFilename, fileSystem);

        std::cout << "Done" << std::endl;
    }

    int UpgradePackBuilderFacade::Result() const
    {
        return result;
    }

    void UpgradePackBuilderFacade::ParseArgument(int& index, int argc, const char* argv[])
    {
        std::string target(ToLower(argv[index]));

        if (index + 1 == argc || target.front() != '-')
            throw UsageException();

        target.erase(0, 1);
        std::string fileName(argv[index + 1]);

        targetAndFiles.push_back(std::make_pair(target, fileName));
        ++index;
    }

    void UpgradePackBuilderFacade::PreBuilder()
    {}

    void UpgradePackBuilderFacade::PostBuilder(UpgradePackBuilder& builder, ImageSigner& signer)
    {}
}
