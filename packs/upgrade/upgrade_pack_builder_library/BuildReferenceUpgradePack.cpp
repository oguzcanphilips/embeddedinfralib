#include "hal/windows/public/FileSystemWin.hpp"
#include "packs/hex_compiler/application/public/BinaryObject.hpp"
#include "mbedtls/memory_buffer_alloc.h"
#include "upgrade_pack_builder_library/BuildUpgradePack.hpp"
#include "upgrade_pack_builder_library/ImageEncryptorAes.hpp"
#include "upgrade_pack_builder_library/ImageSignerEcDsa.hpp"
#include "upgrade_pack_builder_library/UpgradePackBuilder.hpp"
#include "upgrade_pack_builder_library/UpgradePackInputFactory.hpp"
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
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, char* argv[], infra::ConstByteRange aesKey, infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey)
    {
        // Hopefully unneeded, but just in case anyone uses the rand() function seed it with something quasi-random
        std::srand(static_cast<unsigned int>(std::time(nullptr)) * static_cast<unsigned int>(GetCurrentProcessId()));

        // Initialize the MbedTLS memory pool
        unsigned char memory_buf[100000];
        mbedtls_memory_buffer_alloc_init(memory_buf, sizeof(memory_buf));

        try
        {
            if (argc < 3)
                throw UsageException();

            std::vector<std::pair<std::string, std::string>> targetAndFiles;

            std::string outputFilename = argv[1];
            std::cout << "Creating upgrade pack " << outputFilename << "..." << std::endl;

            bool invalidHeaderVersion = false;
            bool invalidProduct = false;
            bool invalidSignature = false;

            for (int i = 2; i < argc; ++i)
            {
                if (argv[i] == std::string("-invalid_header_version"))
                    invalidHeaderVersion = true;
                else if (argv[i] == std::string("-invalid_product"))
                    invalidProduct = true;
                else if (argv[i] == std::string("-invalid_signature"))
                    invalidSignature = true;
                else
                {
                    std::string target(ToLower(argv[i]));

                    if (i + 1 == argc || target.front() != '-')
                        throw UsageException();

                    target.erase(0, 1);
                    std::string fileName(argv[i + 1]);

                    targetAndFiles.push_back(std::make_pair(target, fileName));
                    ++i;
                }
            }

            application::SecureRandomNumberGenerator randomNumberGenerator;
            hal::FileSystemWin fileSystem;
            application::ImageEncryptorAes imageEncryptorAes(randomNumberGenerator, aesKey);
            application::UpgradePackInputFactory inputFactory(supportedHexTargets, supportedBinaryTargets, fileSystem, imageEncryptorAes);
            application::ImageSignerEcDsa signer(randomNumberGenerator, ecDsa224PublicKey, ecDsa224PrivateKey);

            application::UpgradePackBuilder::HeaderInfo builderHeaderInfo = headerInfo;
            if (invalidProduct)
                builderHeaderInfo.productName = "Unknown Product Name";

            application::UpgradePackBuilder builder(targetAndFiles, builderHeaderInfo, inputFactory, signer);

            if (invalidHeaderVersion)
                reinterpret_cast<UpgradePackHeaderEpilogue*>(builder.UpgradePack().data() + sizeof(UpgradePackHeaderPrologue) + signer.SignatureLength())->headerVersion = 0xff;
            if (invalidSignature)
                std::fill(builder.UpgradePack().begin() + sizeof(UpgradePackHeaderPrologue), builder.UpgradePack().begin() + sizeof(UpgradePackHeaderPrologue) + signer.SignatureLength(), 0xff);

            builder.WriteUpgradePack(outputFilename, fileSystem);

            std::cout << "Done" << std::endl;
        }
        catch (UsageException&)
        {
            std::cout << "Invalid number of arguments" << std::endl;
            std::cout << argv[0] << " OUTPUTFILE [-Target1 INPUTFILE1] [-Target2 INPUTFILE2] [-Target3 INPUTFILE3] [-Target4 INPUTFILE4] ..." << std::endl;
            std::cout << "Targets: ";

            for (auto target : supportedHexTargets)
                std::cout << target << " ";
            for (auto targetAndAddress : supportedBinaryTargets)
                std::cout << targetAndAddress.first << " ";
            std::cout << std::endl;

            return 1;
        }
        catch (application::IncorrectCrcException& exception)
        {
            std::cout << "Incorrect CRC in file " << exception.file << " at line " << exception.line << std::endl;
            return 1;
        }
        catch (application::NoEndOfFileException& exception)
        {
            std::cout << "No end of file found in file " << exception.file << " at line " << exception.line << std::endl;
            return 1;
        }
        catch (application::DataAfterEndOfFileException& exception)
        {
            std::cout << "Data found after end of file in file " << exception.file << " at line " << exception.line << std::endl;
            return 1;
        }
        catch (application::UnknownRecordException& exception)
        {
            std::cout << "Unknown record in file " << exception.file << " at line " << exception.line << std::endl;
            return 1;
        }
        catch (application::RecordTooShortException& exception)
        {
            std::cout << "Record too short in file " << exception.file << " at line " << exception.line << std::endl;
            return 1;
        }
        catch (application::RecordTooLongException& exception)
        {
            std::cout << "Record too long in file " << exception.file << " at line " << exception.line << std::endl;
            return 1;
        }
        catch (application::OverwriteException& exception)
        {
            std::cout << "Contents specified twice for memory location at address 0x" << std::hex << std::setw(8) << std::setfill('0') << exception.position << std::endl;
            return 1;
        }
        catch (hal::CannotOpenFileException& exception)
        {
            std::cout << "Cannot open file " << exception.name << std::endl;
            return 1;
        }
        catch (application::SignatureDoesNotVerifyException&)
        {
            std::cout << "Signature does not verify" << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
            return 1;
        }

        return 0;
    }
}
