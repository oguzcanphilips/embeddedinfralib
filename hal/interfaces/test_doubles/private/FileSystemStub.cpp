#include "hal/interfaces/test_doubles/public/FileSystemStub.hpp"
#include "gtest/gtest.h"

namespace hal
{
    FileSystemStub::FileSystemStub(const std::string& name, const std::vector<std::string>& contents)
    {
        files[name] = contents;
    }

    FileSystemStub::FileSystemStub(const std::string& name, const std::vector<uint8_t>& contents)
    {
        binaryFiles[name] = contents;
    }

    std::vector<std::string> FileSystemStub::ReadFile(const std::string& name)
    {
        return files[name];
    }

    void FileSystemStub::WriteFile(const std::string& name, const std::vector<std::string>& contents)
    {
        files[name] = contents;
    }

    std::vector<uint8_t> FileSystemStub::ReadBinaryFile(const std::string& name)
    {
        return binaryFiles[name];
    }

    void FileSystemStub::WriteBinaryFile(const std::string& name, const std::vector<uint8_t>& contents)
    {
        binaryFiles[name] = contents;
    }

}
