#include "hal/windows/public/FileSystemWin.hpp"
#include <fstream>
#include <iterator>

namespace hal
{
    std::vector<std::string> FileSystemWin::ReadFile(const std::string& name)
    {
        std::ifstream input(name);
        if (!input)
            throw CannotOpenFileException(name);

        std::vector<std::string> result;

        while (input)
        {
            std::string line;
            std::getline(input, line);
            result.push_back(line);
        }

        return result;
    }

    void FileSystemWin::WriteFile(const std::string& name, const std::vector<std::string>& contents)
    {
        std::ofstream output(name);

        if (!output)
            throw CannotOpenFileException(name);

        for (std::string line : contents)
            output << line << std::endl;
    }

    std::vector<uint8_t> FileSystemWin::ReadBinaryFile(const std::string& name)
    {
        std::ifstream input(name, std::ios::binary);
        if (!input)
            throw CannotOpenFileException(name);

        std::vector<char> data{ std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
        return std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(data.data()), reinterpret_cast<const uint8_t*>(data.data() + data.size()));
    }

    void FileSystemWin::WriteBinaryFile(const std::string& name, const std::vector<uint8_t>& contents)
    {
        std::ofstream output(name, std::ios::binary);

        if (!output)
            throw CannotOpenFileException(name);

        std::copy(contents.data(), (contents.data() + contents.size()), std::ostreambuf_iterator<char>(output));
    }
}
