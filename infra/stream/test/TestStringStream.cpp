#include "gtest/gtest.h"
#include "infra/stream/public/StdStringStream.hpp"
#include "infra/stream/public/StringStream.hpp"
#include "infra/stream/public/MemoryStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

TEST(StringStreamTest, ExtractDecimal)
{
    infra::BoundedString::WithStorage<10> string("12");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> value;
    EXPECT_EQ(12, value);
}

TEST(StringStreamTest, ExtractHex)
{
    infra::BoundedString::WithStorage<10> string("ab");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xab, value);
}

TEST(StringStreamTest, ExtractSmallValue)
{
    infra::BoundedString::WithStorage<10> string("a");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xa, value);
}

TEST(StringStreamTest, ExtractDelimitedValue)
{
    infra::BoundedString::WithStorage<10> string("a ");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xa, value);
}

TEST(StringStreamTest, ExtractPartialValue)
{
    infra::BoundedString::WithStorage<10> string("abcd");
    infra::StringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> infra::Width(1) >> value;
    EXPECT_EQ(0xa, value);
}

TEST(StringStreamTest, ExtractUint16)
{
    infra::BoundedString::WithStorage<10> string("abcd");
    infra::StringInputStream stream(string);

    uint16_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xabcd, value);
}

TEST(StringStreamTest, ExtractUint32)
{
    infra::BoundedString::WithStorage<10> string("abcd0123");
    infra::StringInputStream stream(string);

    uint32_t value;
    stream >> infra::hex >> value;
    EXPECT_EQ(0xabcd0123, value);
}

TEST(StringStreamTest, StdStringInputStream)
{
    std::string string("abcd");
    infra::StdStringInputStream stream(string);

    uint8_t value;
    stream >> infra::hex >> infra::Width(1) >> value;
    EXPECT_EQ(0xa, value);
}

TEST(StringStreamTest, ExtractHexWithOverflow)
{
    infra::StringInputStream::WithStorage<2> stream(infra::softFail);

    uint8_t v(1);
    stream >> infra::hex >> v;
    EXPECT_EQ(0, v);
    EXPECT_TRUE(stream.HasFailed());
}

TEST(StringStreamTest, StringOutputStreamResetFail)
{
    infra::StringInputStream::WithStorage<2> stream(infra::softFail);

    uint8_t v(1);
    stream >> infra::hex >> v;
    ASSERT_TRUE(stream.HasFailed());
    stream.ResetFail();
    EXPECT_FALSE(stream.HasFailed());
}

TEST(StringStreamTest, ExtractHexFrowStdStringInputStreamWithOverflow)
{
    infra::StdStringInputStream::WithStorage stream(infra::softFail);

    uint8_t v(1);
    stream >> infra::hex >> v;
    EXPECT_EQ(0, v);
    EXPECT_TRUE(stream.HasFailed());
}

TEST(StringStreamTest, StdStringOutputStreamResetFail)
{
    infra::StdStringInputStream::WithStorage stream(infra::softFail);

    uint8_t v(1);
    stream >> infra::hex >> v;
    ASSERT_TRUE(stream.HasFailed());
    stream.ResetFail();
    EXPECT_FALSE(stream.HasFailed());
}

TEST(StringStreamTest, ExtractHexWithoutGoodCharacters)
{
    infra::StringInputStream::WithStorage<2> stream(infra::inPlace, "k", infra::softFail);

    uint8_t v(1);
    stream >> infra::hex >> v;
    EXPECT_EQ(0, v);
    EXPECT_TRUE(stream.HasFailed());
}

TEST(StringStreamTest, ExtractStringLiteral)
{
    infra::StdStringInputStream::WithStorage stream(infra::inPlace, "abcd");

    stream >> "abcd";
    EXPECT_TRUE(stream.Storage().empty());
}
