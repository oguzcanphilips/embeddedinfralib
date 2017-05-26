#include  "hal/interfaces/FileSystem.hpp"

namespace hal
{
    CannotOpenFileException::CannotOpenFileException(const std::string& name)
        : name(name)
    {}
}