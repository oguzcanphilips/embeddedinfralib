#include  "hal/interfaces/public/FileSystem.hpp"

namespace hal
{
    CannotOpenFileException::CannotOpenFileException(const std::string& name)
        : name(name)
    {}
}