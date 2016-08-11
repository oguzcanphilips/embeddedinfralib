#ifndef HAL_STUB_FILE_SYSTEM_STUB_HPP
#define HAL_STUB_FILE_SYSTEM_STUB_HPP

#include "hal/interfaces/public/FileSystem.hpp"
#include <map>

namespace hal
{
    class FileSystemStub
        : public hal::FileSystem
    {
    public:
        FileSystemStub() = default;
        FileSystemStub(const std::string& name, const std::vector<std::string>& contents);
        FileSystemStub(const std::string& name, const std::vector<uint8_t>& contents);

        virtual std::vector<std::string> ReadFile(const std::string& name) override;
        virtual void WriteFile(const std::string& name, const std::vector<std::string>& contents) override;

        virtual std::vector<uint8_t> ReadBinaryFile(const std::string& name) override;
        virtual void WriteBinaryFile(const std::string& name, const std::vector<uint8_t>& contents) override;

        std::map<std::string, std::vector<std::string>> files;
        std::map<std::string, std::vector<uint8_t>> binaryFiles;
    };
}

#endif
