#include "mbedtls/aes.h"
#include "packs/upgrade/pack_builder/public/ImageEncryptorAes.hpp"
#include <cassert>
#include <algorithm>
#include <array>

extern "C"
{
#include "TinyAes.h"
}

namespace application
{
    ImageEncryptorAes::ImageEncryptorAes(RandomNumberGenerator& randomNumberGenerator, infra::ConstByteRange key)
        : randomNumberGenerator(randomNumberGenerator)
        , key(key)
    {}

    uint32_t ImageEncryptorAes::EncryptionAndMacMethod() const
    {
        return encryptionAndMacMethod;
    }

    std::vector<uint8_t> ImageEncryptorAes::Secure(const std::vector<uint8_t>& data) const
    {
        std::vector<uint8_t> counter = randomNumberGenerator.Generate(blockLength);

        mbedtls_aes_context ctx;
        mbedtls_aes_init(&ctx);
        mbedtls_aes_setkey_enc(&ctx, key.begin(), key.size() * 8);

        std::vector<uint8_t> result = counter;
        result.resize(result.size() + data.size(), 0);

        size_t offset = 0;
        std::array<uint8_t, blockLength> stream_block = {};
        int ret = mbedtls_aes_crypt_ctr(&ctx, data.size(), &offset, counter.data(), stream_block.data(), data.data(), result.data() + blockLength);     //TICS !INT#030
        if (ret != 0)
            throw std::exception("AES encryption failed");

        if (!CheckDecryption(data, result))
            throw std::exception("AES decryption check failed");

        return result;
    }

    bool ImageEncryptorAes::CheckDecryption(const std::vector<uint8_t>& original, const std::vector<uint8_t>& encrypted) const
    {
        std::vector<uint8_t> counter(encrypted.begin(), encrypted.begin() + blockLength);
        std::vector<uint8_t> currentStreamBlock(counter.size(), 0);
        std::size_t currentStreamBlockOffset = currentStreamBlock.size();

        std::vector<uint8_t> decrypted(encrypted.begin() + blockLength, encrypted.end());
        infra::ByteRange data(decrypted.data(), decrypted.data() + decrypted.size());
        while (!data.empty())
        {
            if (currentStreamBlockOffset == currentStreamBlock.size())
            {
                AES128_ECB_encrypt(counter.data(), key.begin(), currentStreamBlock.data());
                currentStreamBlockOffset = 0;

                for (std::size_t i = counter.size(); i != 0; --i)
                    if (++counter[i - 1] != 0)
                        break;
            }

            data.front() ^= currentStreamBlock[currentStreamBlockOffset];
            data.pop_front();
            ++currentStreamBlockOffset;
        }

        return mbedtls_aes_self_test(0) == 0 && decrypted == original;
    }
}
