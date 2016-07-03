#include "gtest/gtest.h"
#include "upgrade_pack_builder_library/ImageAuthenticatorHmac.hpp"
#include <array>

class TestImageAuthenticatorHmac
    : public testing::Test
{
public:
    TestImageAuthenticatorHmac()
        : authenticator(hmacKey)
    {}

    application::ImageAuthenticatorHmac authenticator;

    const std::array<uint8_t, 32> hmacKey = std::array<uint8_t, 32>{ {
        0xeb, 0x57, 0xed, 0x0c, 0x74, 0xbb, 0xba, 0x99,
        0x79, 0xca, 0x80, 0x9e, 0x5d, 0x08, 0xfb, 0x6f,
        0x91, 0xc3, 0x2e, 0x32, 0x27, 0x14, 0x6a, 0x83,
        0x5e, 0x71, 0x71, 0x98, 0xed, 0xe8, 0xa3, 0xa5
    } };
};

TEST_F(TestImageAuthenticatorHmac, Secure)
{
    EXPECT_EQ((std::vector<uint8_t>{
        0xd9, 0xa6, 0x35, 0x96, 0x0e, 0x6d, 0x95, 0xb4,
        0x8d, 0x7a, 0x85, 0xe4, 0xc3, 0xe2, 0xb6, 0x20,
        0x88, 0x85, 0x51, 0x13, 0x2a, 0x94, 0xf3, 0x30,
        0x36, 0xc9, 0x2f, 0x54, 0x02, 0xbd, 0x37, 0xed}), authenticator.GenerateMac(std::vector<uint8_t>{1, 2, 3, 4}));
}
