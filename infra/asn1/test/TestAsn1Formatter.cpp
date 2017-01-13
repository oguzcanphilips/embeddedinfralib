#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/asn1/public/Asn1Formatter.hpp"
#include "infra/stream/public/ByteOutputStream.hpp"

TEST(Asn1ObjectFormatter, construction_results_in_empty_object)
{
    infra::ByteOutputStream::WithStorage<4> stream;
    stream << 0;

    infra::Asn1Formatter formatter(stream);

    EXPECT_EQ(false, formatter.HasFailed());
    ASSERT_THAT(stream.Storage(), testing::Each(testing::Eq(0)));
}

TEST(Asn1ObjectFormatter, add_uint32)
{
    infra::ByteOutputStream::WithStorage<6> stream;
    infra::Asn1Formatter formatter(stream);

    formatter.Add(0xAABBAABB);

    EXPECT_EQ(false, formatter.HasFailed());
    ASSERT_THAT(stream.Storage(), testing::ElementsAre(0x02, 0x04, 0xBB, 0xAA, 0xBB, 0xAA));
}

TEST(Asn1ObjectFormatter, add_uint8)
{
    infra::ByteOutputStream::WithStorage<3> stream;
    infra::Asn1Formatter formatter(stream);

    formatter.Add(static_cast<uint8_t>(0xAB));

    EXPECT_EQ(false, formatter.HasFailed());
    ASSERT_THAT(stream.Storage(), testing::ElementsAre(0x02, 0x01, 0xAB));
}

TEST(Asn1ObjectFormatter, add_bignum)
{
    infra::ByteOutputStream::WithStorage<6> stream;
    infra::Asn1Formatter formatter(stream);

    std::array<uint8_t, 4> data = { 0xAB, 0xBA, 0xBA, 0xAB };

    formatter.AddBigNumber(data);

    EXPECT_EQ(false, formatter.HasFailed());
    ASSERT_THAT(stream.Storage(), testing::ElementsAre(0x02, 0x04, 0xAB, 0xBA, 0xBA, 0xAB));
}

TEST(Asn1ObjectFormatter, start_sequence)
{
    infra::ByteOutputStream::WithStorage<8> stream;
    infra::Asn1Formatter formatter(stream);
    {
        auto sequence = formatter.StartSequence();
        sequence.Add(0xFFFFFFFF);
    }

    EXPECT_EQ(false, formatter.HasFailed());
    ASSERT_THAT(stream.Storage(), testing::ElementsAre(0x20 | 0x10, 0x06, 0x02, 0x04, 0xFF, 0xFF, 0xFF, 0xFF));
}

TEST(Asn1ObjectFormatter, add_context_specific)
{
    infra::ByteOutputStream::WithStorage<4> stream;
    infra::Asn1Formatter formatter(stream);

    std::array<uint8_t, 2> data = { 0xAB, 0xBA };

    formatter.AddContextSpecific(data);

    EXPECT_EQ(false, formatter.HasFailed());
    ASSERT_THAT(stream.Storage(), testing::ElementsAre(0x20 | 0x80, 0x02, 0xAB, 0xBA));
}

TEST(Asn1ObjectFormatter, add_object_id)
{
    infra::ByteOutputStream::WithStorage<4> stream;
    infra::Asn1Formatter formatter(stream);

    std::array<uint8_t, 2> data = { 0xAB, 0xBA };

    formatter.AddObjectId(data);

    EXPECT_EQ(false, formatter.HasFailed());
    ASSERT_THAT(stream.Storage(), testing::ElementsAre(0x06, 0x02, 0xAB, 0xBA));
}

TEST(Asn1ObjectFormatter, add_empty_optional)
{
    infra::ByteOutputStream::WithStorage<2> stream;
    infra::Asn1Formatter formatter(stream);

    formatter.AddOptional<uint32_t>(infra::none);

    EXPECT_EQ(false, formatter.HasFailed());
    ASSERT_THAT(stream.Storage(), testing::ElementsAre(0x05, 0x00));
}

TEST(Asn1ObjectFormatter, add_non_empty_optional)
{
    infra::ByteOutputStream::WithStorage<3> stream;
    infra::Asn1Formatter formatter(stream);

    infra::Optional<uint8_t> value(infra::inPlace, 0xAB);
    formatter.AddOptional<uint8_t>(value);

    EXPECT_EQ(false, formatter.HasFailed());
    ASSERT_THAT(stream.Storage(), testing::ElementsAre(0x02, 0x01, 0xAB));
}
