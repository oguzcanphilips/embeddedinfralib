#ifndef UPGRADE_PACK_BUILD_LIBRARY_IMAGE_ENCRYPTOR_XTEA_HPP
#define UPGRADE_PACK_BUILD_LIBRARY_IMAGE_ENCRYPTOR_XTEA_HPP

#include "upgrade_pack_builder_library/ImageAuthenticatorHmac.hpp"
#include "upgrade_pack_builder_library/ImageSecurity.hpp"
#include "upgrade_pack_builder_library/RandomNumberGenerator.hpp"
#include <cstdint>
#include <vector>

namespace application
{
    class ImageEncryptorXtea
    {
    public:
        static const std::size_t blockLength = 8;

        ImageEncryptorXtea(RandomNumberGenerator& randomNumberGenerator, infra::ConstByteRange key);

        std::vector<uint8_t> Secure(const std::vector<uint8_t>& data) const;

    private:
        bool CheckDecryption(const std::vector<uint8_t>& original, const std::vector<uint8_t>& encrypted) const;

        RandomNumberGenerator& randomNumberGenerator;
        infra::ConstByteRange key;
    };
}

#endif
