#include "upgrade_pack_builder_library/ImageEncryptorNone.hpp"

namespace application
{
    uint32_t ImageEncryptorNone::EncryptionAndMacMethod() const
    {
        return encryptionAndMacMethod;
    }

    std::vector<uint8_t> ImageEncryptorNone::Secure(const std::vector<uint8_t>& data) const
    {
        return data;
    }
}
