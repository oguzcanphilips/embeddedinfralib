#ifndef HAL_WINDOWS_FILE_SYSTEM_WIN_HPP
#define HAL_WINDOWS_FILE_SYSTEM_WIN_HPP

#include "hal/interfaces/FileSystem.hpp"

namespace hal
{    
    class FileSystemWin
        : public hal::FileSystem
    {
    public:
        virtual std::vector<std::string> ReadFile(const std::string& name) override;
        virtual void WriteFile(const std::string& name, const std::vector<std::string>& contents) override;

        virtual std::vector<uint8_t> ReadBinaryFile(const std::string& name) override;
        virtual void WriteBinaryFile(const std::string& name, const std::vector<uint8_t>& contents) override;
    };
}

#endif