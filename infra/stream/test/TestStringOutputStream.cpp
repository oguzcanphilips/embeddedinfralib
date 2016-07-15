#include "gtest/gtest.h"
#include "infra/stream/public/StringOutputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

TEST(StringOutputStreamTest, stream_byte)
{
    infra::StringOutputStream::WithStorage<2> stream;

    stream << uint8_t(12);
    EXPECT_EQ("12", stream.Storage());
}

TEST(StringOutputStreamTest, stream_from_bounded_string)
{
    infra::BoundedString::WithStorage<10> s = "abcd";
    infra::StringOutputStream stream(s);

    stream << uint8_t(12) << infra::data << uint8_t('a');

    EXPECT_EQ("abcd12a", s);
}

TEST(StringOutputStreamTest, stream_literal)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << "abcd";

    EXPECT_EQ("abcd", stream.Storage());
}

TEST(StringOutputStreamTest, stream_literal_in_hex_stream)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << "abcd";

    EXPECT_EQ("abcd", stream.Storage());
}

TEST(StringOutputStreamTest, stream_character)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << 'a';

    EXPECT_EQ("a", stream.Storage());
}

TEST(StringOutputStreamTest, stream_uint8)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << uint8_t(255);

    EXPECT_EQ("255", stream.Storage());
}

TEST(StringOutputStreamTest, stream_int8)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << int8_t(127);

    EXPECT_EQ("127", stream.Storage());
}

TEST(StringOutputStreamTest, stream_negative_int8)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << int8_t(-128);

    EXPECT_EQ("-128", stream.Storage());
}

TEST(StringOutputStreamTest, stream_int8_with_leading_zeroes)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::Width(5,'0') << int8_t(127);

    EXPECT_EQ("00127", stream.Storage());
}

TEST(StringOutputStreamTest, stream_int8_with_smaller_width)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::Width(2) << int8_t(127);

    EXPECT_EQ("127", stream.Storage());
}

TEST(StringOutputStreamTest, stream_short_hex)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << uint8_t(10);

    EXPECT_EQ("a", stream.Storage());
}

TEST(StringOutputStreamTest, stream_longer_hex)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << uint8_t(0x1A);
    EXPECT_EQ("1a", stream.Storage());
}

TEST(StringOutputStreamTest, stream_negative_hex)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << int8_t(-0x1A);
    EXPECT_EQ("-1a", stream.Storage());
}

TEST(StringOutputStreamTest, stream_hex_with_leading_zeroes)
{
    infra::StringOutputStream::WithStorage<10> stream;

    stream << infra::hex << infra::Width(4,'0') << uint8_t(0x1A);
    EXPECT_EQ("001a", stream.Storage());
}

TEST(StringOutputStreamTest, overflow)
{
    infra::StringOutputStream::WithStorage<2> stream(infra::softFail);

    stream << "abc";
    EXPECT_EQ("ab", stream.Storage());
    EXPECT_TRUE(stream.HasFailed());
}

TEST(StringOutputStreamTest, overflow_twice)
{
    infra::StringOutputStream::WithStorage<2> stream(infra::softFail);

    stream << "abc" << "def";
    EXPECT_EQ("ab", stream.Storage());
    EXPECT_TRUE(stream.HasFailed());
}

TEST(StringOutputStreamTest, format_simple_string)
{
    infra::StringOutputStream::WithStorage<64> stream;

    stream.Format("simple");
    EXPECT_EQ("simple", stream.Storage());
}

TEST(StringOutputStreamTest, format_string_with_one_parameter)
{
    infra::StringOutputStream::WithStorage<64> stream;

    stream.Format("%1%", 1);
    EXPECT_EQ("1", stream.Storage());
}

TEST(StringOutputStreamTest, format_string_with_two_parameters)
{
    infra::StringOutputStream::WithStorage<64> stream;

    stream.Format("%1% %2%", 5, 'a');
    EXPECT_EQ("5 a", stream.Storage());
}

TEST(StringOutputStreamTest, format_string_with_string)
{
    infra::StringOutputStream::WithStorage<64> stream;

    stream.Format("%1%", "bla");
    EXPECT_EQ("bla", stream.Storage());
}

TEST(StringOutputStreamTest, format_string_with_invalid_param_specifier)
{
    infra::StringOutputStream::WithStorage<64> stream;

    stream.Format("%bla");
    EXPECT_EQ("bla", stream.Storage());
}

TEST(StringOutputStreamTest, format_string_with_incomplete_param_specifier)
{
    infra::StringOutputStream::WithStorage<64> stream;

    stream.Format("%1", "bla");
    EXPECT_EQ("", stream.Storage());
}

TEST(StringOutputStreamTest, format_string_with_out_of_range_index)
{
    infra::StringOutputStream::WithStorage<64> stream;

    stream.Format("%3%", 1, 2);
    EXPECT_EQ("", stream.Storage());
}

struct MyObject
{
    explicit MyObject(int) {}
    MyObject(const MyObject& other) = delete;
    MyObject& operator=(const MyObject& other) = delete;

    friend infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const MyObject& object)
    {
        stream << "MyObject!";
        return stream;
    }
};

TEST(StringOutputStreamTest, format_custom_parameter)
{
    infra::StringOutputStream::WithStorage<64> stream;

    MyObject myObject(5);
    stream.Format("%1%", myObject);
    EXPECT_EQ("MyObject!", stream.Storage());
}

TEST(StringOutputStreamTest, stream_byte_range_as_ascii)
{
    infra::StringOutputStream::WithStorage<64> stream;

    std::array<uint8_t, 4> data = { 1, 2, 'a', 'b' };
    stream << infra::AsAscii(infra::ByteRange(data));
    EXPECT_EQ("..ab", stream.Storage());
}

TEST(StringOutputStreamTest, stream_byte_range_as_hex)
{
    infra::StringOutputStream::WithStorage<64> stream;

    std::array<uint8_t, 4> data = { 1, 2, 0x30, 0x40 };
    stream << infra::AsHex(infra::ByteRange(data));
    EXPECT_EQ("01023040", stream.Storage());
}
