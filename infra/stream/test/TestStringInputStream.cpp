#include "gtest/gtest.h"
#include "infra/stream/StringInputStream.hpp"
#include "infra/util/BoundedString.hpp"
#include <cstdint>

TEST(StringInputStreamTest, ExtractDecimal)
{
    infra::BoundedString::WithStorage<10> string("12");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> value;
    EXPECT_EQ(12, value);
}

TEST(StringInputStreamTest, ExtractHex)
{
    infra::BoundedString::WithStorage<10> string("ab");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xab, value);
}

TEST(StringInputStreamTest, ExtractSmallValue)
{
    infra::BoundedString::WithStorage<10> string("a");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xa, value);
}

TEST(StringInputStreamTest, ExtractDelimitedValue)
{
    infra::BoundedString::WithStorage<10> string("a ");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xa, value);
}

TEST(StringInputStreamTest, ExtractPartialValue)
{
    infra::BoundedString::WithStorage<10> string("abcd");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> infra::Width(1) >> value;
    EXPECT_EQ(0xa, value);
}

TEST(StringInputStreamTest, ExtractUint16)
{
    infra::BoundedString::WithStorage<10> string("abcd");
    infra::StringInputStream stream(string);

    uint16_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xabcd, value);
}

TEST(StringInputStreamTest, ExtractUint32)
{
    infra::BoundedString::WithStorage<10> string("abcd0123");
    infra::StringInputStream stream(string);

    uint32_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xabcd0123, value);
}

TEST(StringInputStreamTest, ExtractHexWithOverflow)
{
    infra::BoundedString::WithStorage<1> string("");
    infra::StringInputStream stream(string, infra::softFail);

    uint8_t v(1);
    stream >> infra::hex >> v;
    EXPECT_EQ(0, v);
    EXPECT_TRUE(stream.Failed());
}

TEST(StringInputStreamTest, ExtractHexWithoutGoodCharacters)
{
    infra::StringInputStream stream(infra::BoundedConstString("k"), infra::softFail);

    uint8_t v(1);
    stream >> infra::hex >> v;
    EXPECT_EQ(0, v);
    EXPECT_TRUE(stream.Failed());
}
