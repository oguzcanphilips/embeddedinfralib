#include "mbedtls/md.h"
#include "mbedtls/sha256.h"
#include "upgrade/pack_builder/ImageAuthenticatorHmac.hpp"
#include <algorithm>
#include <fstream>
#include <iterator>

namespace application
{
    ImageAuthenticatorHmac::ImageAuthenticatorHmac(infra::ConstByteRange key)
        : key(key)
    {}

    std::vector<uint8_t> ImageAuthenticatorHmac::GenerateMac(const std::vector<uint8_t>& data) const
    {
        std::vector<uint8_t> mac(32, 0);

        mbedtls2_md_context_t ctx;
        mbedtls2_md_init(&ctx);
        mbedtls2_md_setup(&ctx, mbedtls2_md_info_from_type(MBEDTLS_MD_SHA256), 1);                                        //TICS !COV_CPP_CHECKED_RETURN_01
        mbedtls2_md_hmac_starts(&ctx, key.begin(), key.size());
        mbedtls2_md_hmac_update(&ctx, data.data(), data.size());
        mbedtls2_md_hmac_finish(&ctx, mac.data());
        mbedtls2_md_free(&ctx);

        return mac;
    }
}
