#include "gtest/gtest.h"
#include "infra/stream/ByteInputStream.hpp"
#include <array>

TEST(ByteInputStreamTest, StreamFromRange)
{
    std::array<uint8_t, 4> from = { 0, 1, 2, 3 };

    struct To
    {
        uint8_t a;
        uint8_t b;

        bool operator==(const To& other) const { return a == other.a && b == other.b; };
    } to = { 4, 5 };

    infra::ByteInputStream stream(from);

    EXPECT_EQ(from, stream.Remaining());

    stream >> to;
    EXPECT_EQ((To{ 0, 1 }), to);
    EXPECT_FALSE(stream.Empty());

    stream >> to;
    EXPECT_EQ((To{ 2, 3 }), to);
    EXPECT_TRUE(stream.Empty());
    EXPECT_EQ(from, stream.Processed());
}

TEST(ByteInputStreamTest, StreamToMemoryRange)
{
    std::array<uint8_t, 2> to = { 0, 1 };
    std::array<uint8_t, 4> buffer = { 2, 3, 4, 5 };

    infra::ByteInputStream stream(buffer);
    stream >> to;

    EXPECT_EQ((std::array<uint8_t, 2>{{ 2, 3 }}), to);
}
