#include "gtest/gtest.h"
#include "infra/stream/public/ByteOutputStream.hpp"

TEST(ByteOutputStreamTest, StreamToRange)
{
    struct From
    {
        uint8_t a;
        uint8_t b;
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

TEST(ByteOutputStreamTest, StreamForward)
{
    std::array<uint8_t, 2> from = { 10, 11 };
    std::array<uint8_t, 8> buffer = { 0, 1, 2, 3, 4, 5, 6, 7 };

    infra::ByteOutputStream stream(buffer);
    stream << infra::ByteRange(from);
    stream << infra::ForwardStream(3);
    stream << infra::ByteRange(from);

    EXPECT_EQ((std::array<uint8_t, 8>{{ 10, 11, 2, 3, 4, 10, 11, 7 }}), buffer);
}

TEST(ByteOutputStreamTest, WithStorage)
{
    infra::ByteOutputStream::WithStorage<5> stream;
    stream << uint8_t(1) << uint8_t(2) << uint8_t(3);

    EXPECT_EQ((std::array<uint8_t, 3>{{ 1, 2, 3 }}), stream.Processed());
}

TEST(ByteOutputStreamTest, reserve_type)
{
    infra::ByteOutputStream::WithStorage<5> stream;
    stream << uint8_t(1);
    auto reservedSpace = stream.Reserve<uint8_t>();
    stream << uint8_t(3);
    reservedSpace = uint8_t(2);

    EXPECT_EQ((std::array<uint8_t, 3>{ { 1, 2, 3 }}), stream.Processed());
}

TEST(ByteOutputStreamTest, reserve_type_without_space)
{
    infra::ByteOutputStream::WithStorage<2> stream(infra::softFail);
    stream << uint8_t(1);
    auto reservedSpace = stream.Reserve<uint32_t>();
    reservedSpace = uint32_t(32);

    EXPECT_TRUE(stream.HasFailed());
}
