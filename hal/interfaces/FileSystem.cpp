#include  "hal/interfaces/FileSystem.hpp"

namespace hal
{
    CannotOpenFileException::CannotOpenFileException(const hal::filesystem::path& path)
        : path(path)
    {}
}
