#ifndef HEX_COMPILER_COMPILER_HPP
#define HEX_COMPILER_COMPILER_HPP

#include "hal/interfaces/public/FileSystem.hpp"
#include "packs/hex_compiler/application/public/BinaryObject.hpp"
#include "hex_compiler_application_generated/CommandLineParser.hpp"
#include <string>
#include <vector>

namespace application
{
    struct IncorrectUsageException
        : std::exception
    {};

    class Compiler
        : private CommandLineParser
    {
    public:
        Compiler(hal::FileSystem& fileSystem);

        void AddParameter(const std::string& parameter);
        void Compile();

    protected:
        virtual void AddInputFile() override;
        virtual void SetOutputFile() override;
        virtual void ProcessFiles() override;
        virtual void SetOffset() override;
        virtual void ReportUsage() override;
        virtual void SetHexFormat() override;
        virtual void SetBinaryFormat() override;

    private:
        hal::FileSystem& fileSystem;
        BinaryObject object;
        std::string fileName;
        std::string outputFileName;
        uint32_t number = 0;
        uint32_t offset = 0;
        bool hexFormat = true;
    };
}

#endif
