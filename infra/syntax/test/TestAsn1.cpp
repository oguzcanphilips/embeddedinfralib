#include "gtest/gtest.h"
#include "infra/syntax/Asn1.hpp"

TEST(Asn1ObjectFormatter, get_integer)
{
    std::array<uint8_t, 3> data = { { 0x02, 0x01, 0xab } };
    infra::Asn1Sequence sequence(data);

    EXPECT_EQ(infra::ConstByteRange(data.data() + 2, data.data() + 3), sequence.front().Integer());
}

TEST(Asn1ObjectFormatter, get_sequence)
{
    std::array<uint8_t, 8> data = { { 0x30, 0x06, 0x02, 0x04, 0xFF, 0xFF, 0xFF, 0xFF } };
    infra::Asn1Sequence sequence(data);

    EXPECT_EQ(infra::Asn1Sequence(infra::ConstByteRange(data.data() + 2, data.data() + 8)), sequence.begin()->Sequence());
}

TEST(Asn1ObjectFormatter, get_second_integer)
{
    std::array<uint8_t, 6> data = { { 0x02, 0x01, 0xab, 0x02, 0x01, 0xcd } };
    infra::Asn1Sequence sequence(data);

    EXPECT_EQ(infra::ConstByteRange(data.data() + 5, data.data() + 6), std::next(sequence.begin())->Integer());
    EXPECT_EQ(infra::ConstByteRange(data.data() + 5, data.data() + 6), sequence[1].Integer());
}
