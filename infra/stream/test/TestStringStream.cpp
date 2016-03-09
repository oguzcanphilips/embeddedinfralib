//#include "gtest/gtest.h"
//#include "infra/stream/public/StdStringStream.hpp"
//#include "infra/stream/public/StringStream.hpp"
//#include "infra/stream/public/MemoryStream.hpp"
//#include "infra/util/public/BoundedString.hpp"
//#include <cstdint>
//
//TEST(StringStreamTest, StreamByte)
//{
//    infra::StringOutputStream::WithStorage<2> stream;
//
//    stream << uint8_t(12);
//    EXPECT_EQ("12", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamToTextStream)
//{
//    infra::ByteOutputStream<uint8_t>::WithStorage<2> stream;
//
//    stream << infra::text << uint8_t(12);
//    EXPECT_EQ((std::array<uint8_t, 2>{{ '1', '2' }}), stream.Processed());
//}
//
//TEST(StringStreamTest, TextStreamToStream)
//{
//    infra::StringOutputStream::WithStorage<2> stream;
//
//    stream << infra::data << uint8_t(012);
//    EXPECT_EQ("\012", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamToTextStreamToStream)
//{
//    infra::ByteOutputStream<uint8_t>::WithStorage<3> stream;
//
//    stream << infra::text << uint8_t(12) << infra::data << uint8_t(12);
//    EXPECT_EQ((std::array<uint8_t, 3>{{ '1', '2', 12 }}), stream.Processed());
//}
//
//TEST(StringStreamTest, StreamFromBoundedString)
//{
//    infra::BoundedString::WithStorage<10> s = "abcd";
//    infra::StringOutputStream stream(s);
//
//    stream << uint8_t(12) << infra::data << uint8_t('a');
//
//    EXPECT_EQ("abcd12a", s);
//}
//
//TEST(StringStreamTest, StreamLiteral)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << "abcd";
//
//    EXPECT_EQ("abcd", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamLiteralInHexStream)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << infra::hex << "abcd";
//
//    EXPECT_EQ("abcd", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamCharacter)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << 'a';
//
//    EXPECT_EQ("a", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamUint8)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << uint8_t(255);
//
//    EXPECT_EQ("255", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamInt8)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << int8_t(127);
//
//    EXPECT_EQ("127", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamNegativeInt8)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << int8_t(-128);
//
//    EXPECT_EQ("-128", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamInt8WithLeadingZeroes)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << infra::Width(5) << int8_t(127);
//
//    EXPECT_EQ("00127", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamInt8WithSmallerWidth)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << infra::Width(2) << int8_t(127);
//
//    EXPECT_EQ("127", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamShortHex)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << infra::hex << uint8_t(10);
//
//    EXPECT_EQ("a", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamLongerHex)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << infra::hex << uint8_t(0x1A);
//    EXPECT_EQ("1a", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamNegativeHex)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << infra::hex << int8_t(-0x1A);
//    EXPECT_EQ("-1a", stream.Storage());
//}
//
//TEST(StringStreamTest, StreamHexWithLeadingZeroes)
//{
//    infra::StringOutputStream::WithStorage<10> stream;
//
//    stream << infra::hex << infra::Width(4) << uint8_t(0x1A);
//    EXPECT_EQ("001a", stream.Storage());
//}
//
//TEST(StringStreamTest, Overflow)
//{
//    infra::StringOutputStream::WithStorage<2> stream(infra::softFail);
//
//    stream << "abc";
//    EXPECT_EQ("ab", stream.Storage());
//    EXPECT_TRUE(stream.HasFailed());
//}
//
//TEST(StringStreamTest, OverflowTwice)
//{
//    infra::StringOutputStream::WithStorage<2> stream(infra::softFail);
//
//    stream << "abc" << "def";
//    EXPECT_EQ("ab", stream.Storage());
//    EXPECT_TRUE(stream.HasFailed());
//}
//
//TEST(StringStreamTest, ResetFail)
//{
//    infra::StringOutputStream::WithStorage<2> stream(infra::softFail);
//
//    stream << "abc";
//    ASSERT_TRUE(stream.HasFailed());
//    stream.ResetFail();
//    EXPECT_FALSE(stream.HasFailed());
//}
//
//TEST(StringStreamTest, ExtractDecimal)
//{
//    infra::BoundedString::WithStorage<10> string("12");
//    infra::StringInputStream stream(string);
//
//    uint8_t value;
//    stream >> value;
//    EXPECT_EQ(12, value);
//}
//
//TEST(StringStreamTest, ExtractHex)
//{
//    infra::BoundedString::WithStorage<10> string("ab");
//    infra::StringInputStream stream(string);
//
//    uint8_t value;
//    stream >> infra::hex >> value;
//    EXPECT_EQ(0xab, value);
//}
//
//TEST(StringStreamTest, ExtractSmallValue)
//{
//    infra::BoundedString::WithStorage<10> string("a");
//    infra::StringInputStream stream(string);
//
//    uint8_t value;
//    stream >> infra::hex >> value;
//    EXPECT_EQ(0xa, value);
//}
//
//TEST(StringStreamTest, ExtractDelimitedValue)
//{
//    infra::BoundedString::WithStorage<10> string("a ");
//    infra::StringInputStream stream(string);
//
//    uint8_t value;
//    stream >> infra::hex >> value;
//    EXPECT_EQ(0xa, value);
//}
//
//TEST(StringStreamTest, ExtractPartialValue)
//{
//    infra::BoundedString::WithStorage<10> string("abcd");
//    infra::StringInputStream stream(string);
//
//    uint8_t value;
//    stream >> infra::hex >> infra::Width(1) >> value;
//    EXPECT_EQ(0xa, value);
//}
//
//TEST(StringStreamTest, ExtractUint16)
//{
//    infra::BoundedString::WithStorage<10> string("abcd");
//    infra::StringInputStream stream(string);
//
//    uint16_t value;
//    stream >> infra::hex >> value;
//    EXPECT_EQ(0xabcd, value);
//}
//
//TEST(StringStreamTest, ExtractUint32)
//{
//    infra::BoundedString::WithStorage<10> string("abcd0123");
//    infra::StringInputStream stream(string);
//
//    uint32_t value;
//    stream >> infra::hex >> value;
//    EXPECT_EQ(0xabcd0123, value);
//}
//
//TEST(StringStreamTest, StdStringInputStream)
//{
//    std::string string("abcd");
//    infra::StdStringInputStream stream(string);
//
//    uint8_t value;
//    stream >> infra::hex >> infra::Width(1) >> value;
//    EXPECT_EQ(0xa, value);
//}
//
//TEST(StringStreamTest, StdStringOutputStream)
//{
//    infra::StdStringOutputStream::WithStorage stream;
//
//    stream << "abcd";
//
//    EXPECT_EQ("abcd", stream.Storage());
//}
//
//TEST(StringStreamTest, ExtractHexWithOverflow)
//{
//    infra::StringInputStream::WithStorage<2> stream(infra::softFail);
//
//    uint8_t v(1);
//    stream >> infra::hex >> v;
//    EXPECT_EQ(0, v);
//    EXPECT_TRUE(stream.HasFailed());
//}
//
//TEST(StringStreamTest, StringOutputStreamResetFail)
//{
//    infra::StringInputStream::WithStorage<2> stream(infra::softFail);
//
//    uint8_t v(1);
//    stream >> infra::hex >> v;
//    ASSERT_TRUE(stream.HasFailed());
//    stream.ResetFail();
//    EXPECT_FALSE(stream.HasFailed());
//}
//
//TEST(StringStreamTest, ExtractHexFrowStdStringInputStreamWithOverflow)
//{
//    infra::StdStringInputStream::WithStorage stream(infra::softFail);
//
//    uint8_t v(1);
//    stream >> infra::hex >> v;
//    EXPECT_EQ(0, v);
//    EXPECT_TRUE(stream.HasFailed());
//}
//
//TEST(StringStreamTest, StdStringOutputStreamResetFail)
//{
//    infra::StdStringInputStream::WithStorage stream(infra::softFail);
//
//    uint8_t v(1);
//    stream >> infra::hex >> v;
//    ASSERT_TRUE(stream.HasFailed());
//    stream.ResetFail();
//    EXPECT_FALSE(stream.HasFailed());
//}
//
//TEST(StringStreamTest, ExtractHexWithoutGoodCharacters)
//{
//    infra::StringInputStream::WithStorage<2> stream(infra::inPlace, "k", infra::softFail);
//
//    uint8_t v(1);
//    stream >> infra::hex >> v;
//    EXPECT_EQ(0, v);
//    EXPECT_TRUE(stream.HasFailed());
//}
//
//TEST(StringStreamTest, ExtractStringLiteral)
//{
//    infra::StdStringInputStream::WithStorage stream(infra::inPlace, "abcd");
//
//    stream >> "abcd";
//    EXPECT_TRUE(stream.Storage().empty());
//}
