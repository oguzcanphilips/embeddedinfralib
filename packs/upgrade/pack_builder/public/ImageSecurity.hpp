#ifndef UPGRADE_PACK_BUILDER_LIBRARY_IMAGE_SECURITY_HPP
#define UPGRADE_PACK_BUILDER_LIBRARY_IMAGE_SECURITY_HPP

#include <cstdint>
#include <vector>

namespace application
{
    class ImageSecurity
    {
    public:
        virtual uint32_t EncryptionAndMacMethod() const = 0;
        virtual std::vector<uint8_t> Secure(const std::vector<uint8_t>& data) const = 0;
    };
}

#endif
