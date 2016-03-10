#include "gtest/gtest.h"
#include "infra/stream/public/StdStringInputStream.hpp"
#include <cstdint>

TEST(StringInputStreamTest, StdStringInputStream)
{
    std::string string("abcd");
    infra::StdStringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> infra::Width(1) >> value;
    EXPECT_EQ(0xa, value);
}

TEST(StringInputStreamTest, ExtractHexFrowStdStringInputStreamWithOverflow)
{
    infra::StdStringInputStream stream("",infra::softFail);

    uint8_t v(1);
    stream >> infra::hex >> v;
    EXPECT_EQ(0, v);
    EXPECT_TRUE(stream.IsFailed());
}

TEST(StringInputStreamTest, ExtractStringLiteral)
{
    infra::StdStringInputStream stream("abcd");

    stream >> "abcd";
    EXPECT_TRUE(stream.IsEmpty());
}
