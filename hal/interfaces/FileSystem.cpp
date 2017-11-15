#include  "hal/interfaces/FileSystem.hpp"

namespace hal
{
    CannotOpenFileException::CannotOpenFileException(const hal::filesystem::path& path)
        : path(path)
    {}

    EmptyFileException::EmptyFileException(const hal::filesystem::path& path)
        : path(path)
    {}
}
