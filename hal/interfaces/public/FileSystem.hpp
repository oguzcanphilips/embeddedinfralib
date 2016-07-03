#ifndef HAL_INTERFACE_FILE_SYSTEM_HPP
#define HAL_INTERFACE_FILE_SYSTEM_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace hal
{
    class FileSystem
    {
    public:
        virtual std::vector<std::string> ReadFile(const std::string& name) = 0;
        virtual void WriteFile(const std::string& name, const std::vector<std::string>& contents) = 0;

        virtual std::vector<uint8_t> ReadBinaryFile(const std::string& name) = 0;
        virtual void WriteBinaryFile(const std::string& name, const std::vector<uint8_t>& contents) = 0;

    protected:
        ~FileSystem() = default;
    };
}

#endif
