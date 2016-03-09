#include "gtest/gtest.h"
#include "infra/stream/public/ByteOutputStream.hpp"

TEST(ByteOutputStreamTest, StreamToRange)
{
    struct From
    {
        uint8_t a;
        uint8_t b;

        bool operator==(const From& other) const { return a == other.a && b == other.b; };
    } from = { 0, 1 };

    std::array<uint8_t, 4> to = { 2, 3, 4, 5 };

    infra::ByteOutputStream stream(to);
    stream << from;

    EXPECT_EQ((std::array<uint8_t, 4>{{ 0, 1, 4, 5 }}), to);
    EXPECT_EQ((std::vector<uint8_t>{ 4, 5 }), stream.Remaining());
    EXPECT_EQ((std::vector<uint8_t>{ 0, 1 }), stream.Processed());
}

TEST(ByteOutputStreamTest, StreamFromMemoryRange)
{
    std::array<uint8_t, 2> from = { 0, 1 };
    std::array<uint8_t, 4> buffer = { 2, 3, 4, 5 };

    infra::ByteOutputStream stream(buffer);
    stream << infra::ByteRange(from);

    EXPECT_EQ((std::array<uint8_t, 4>{{ 0, 1, 4, 5 }}), buffer);
}

TEST(ByteOutputStreamTest, WithStorage)
{
    infra::ByteOutputStream::WithStorage<5> stream;
    stream << uint8_t(1) << uint8_t(2) << uint8_t(3);

    EXPECT_EQ((std::array<uint8_t, 3>{{ 1, 2, 3 }}), stream.Processed());
}
