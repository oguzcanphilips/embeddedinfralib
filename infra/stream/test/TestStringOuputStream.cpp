#include "gtest/gtest.h"
#include "infra/stream/public/StringOutputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

TEST(StringOuputStreamTest, StreamByte)
{
    infra::StringOutputStream::WithStorage<2> stream;

    stream << uint8_t(12);
    EXPECT_EQ("12", stream.Storage());
}

TEST(StringOuputStreamTest, StreamFromBoundedString)
{
    infra::BoundedString::WithStorage<10> s = "abcd";
    infra::StringOutputStream stream(s);

    stream << uint8_t(12) << infra::data << uint8_t('a');

    EXPECT_EQ("abcd12a", s);
}

TEST(StringOuputStreamTest, StreamLiteral)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << "abcd";

    EXPECT_EQ("abcd", stream.Storage());
}

TEST(StringOuputStreamTest, StreamLiteralInHexStream)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << "abcd";

    EXPECT_EQ("abcd", stream.Storage());
}

TEST(StringOuputStreamTest, StreamCharacter)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << 'a';

    EXPECT_EQ("a", stream.Storage());
}

TEST(StringOuputStreamTest, StreamUint8)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << uint8_t(255);

    EXPECT_EQ("255", stream.Storage());
}

TEST(StringOuputStreamTest, StreamInt8)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << int8_t(127);

    EXPECT_EQ("127", stream.Storage());
}

TEST(StringOuputStreamTest, StreamNegativeInt8)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << int8_t(-128);

    EXPECT_EQ("-128", stream.Storage());
}

TEST(StringOuputStreamTest, StreamInt8WithLeadingZeroes)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::Width(5,'0') << int8_t(127);

    EXPECT_EQ("00127", stream.Storage());
}

TEST(StringOuputStreamTest, StreamInt8WithSmallerWidth)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::Width(2) << int8_t(127);

    EXPECT_EQ("127", stream.Storage());
}

TEST(StringOuputStreamTest, StreamShortHex)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << uint8_t(10);

    EXPECT_EQ("a", stream.Storage());
}

TEST(StringOuputStreamTest, StreamLongerHex)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << uint8_t(0x1A);
    EXPECT_EQ("1a", stream.Storage());
}

TEST(StringOuputStreamTest, StreamNegativeHex)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << int8_t(-0x1A);
    EXPECT_EQ("-1a", stream.Storage());
}

TEST(StringOuputStreamTest, StreamHexWithLeadingZeroes)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << infra::Width(4,'0') << uint8_t(0x1A);
    EXPECT_EQ("001a", stream.Storage());
}

TEST(StringOuputStreamTest, Overflow)
{
    infra::StringOutputStream::WithStorage<2> stream(infra::softFail);

    stream << "abc";
    EXPECT_EQ("ab", stream.Storage());
    EXPECT_TRUE(stream.HasFailed());
}

TEST(StringOuputStreamTest, OverflowTwice)
{
    infra::StringOutputStream::WithStorage<2> stream(infra::softFail);

    stream << "abc" << "def";
    EXPECT_EQ("ab", stream.Storage());
    EXPECT_TRUE(stream.HasFailed());
}
