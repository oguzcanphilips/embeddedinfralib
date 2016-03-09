#include "gtest/gtest.h"
#include "infra/stream/public/MemoryStream.hpp"
#include "infra/stream/public/ByteOutputStream.hpp"

TEST(MemoryStreamTest, StreamFromRange)
{
    std::array<int, 4> from = { 0, 1, 2, 3 };

    struct To
    {
        int a;
        int b;

        bool operator==(const To& other) const { return a == other.a && b == other.b; };
    } to = { 4, 5 };

    infra::MemoryInputStream<int> stream(from);
    stream >> to;

    EXPECT_EQ((std::vector<int>{ 2, 3 }), stream.Remaining());
    EXPECT_EQ((std::vector<int>{ 0, 1 }), stream.Processed());
    EXPECT_EQ((To{ 0, 1 }), to);
}

TEST(MemoryStreamTest, StreamToRange)
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

TEST(MemoryStreamTest, StreamFromMemoryRange)
{
    std::array<uint8_t, 2> from = { 0, 1 };
    std::array<uint8_t, 4> buffer = { 2, 3, 4, 5 };

    infra::ByteOutputStream stream(buffer);
    stream << from;

    EXPECT_EQ((std::array<uint8_t, 4>{{ 0, 1, 4, 5 }}), buffer);
}

TEST(MemoryStreamTest, StreamToMemoryRange)
{
    std::array<int, 2> to = { 0, 1 };
    std::array<int, 4> buffer = { 2, 3, 4, 5 };

    infra::MemoryInputStream<int> stream(buffer);
    stream >> to;

    EXPECT_EQ((std::array<int, 2>{{ 2, 3 }}), to);
}

TEST(MemoryStreamTest, ForwardSkipsBytes)
{
    std::array<int, 2> to = { 0, 1 };
    std::array<int, 4> buffer = { 2, 3, 4, 5 };

    infra::MemoryInputStream<int> stream(buffer);
    stream >> infra::ForwardStream(1) >> to;

    EXPECT_EQ((std::array<int, 2>{{ 3, 4 }}), to);
}

TEST(MemoryStreamTest, WithStorage)
{
    infra::ByteOutputStream::WithStorage<5> stream;
    stream << uint8_t(1) << uint8_t(2) << uint8_t(3);

    EXPECT_EQ((std::array<uint8_t, 3>{{ 1, 2, 3 }}), stream.Processed());
}
