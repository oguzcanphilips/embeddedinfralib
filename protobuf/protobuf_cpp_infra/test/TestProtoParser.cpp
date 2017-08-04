#include "gmock/gmock.h"
#include "infra/stream/ByteInputStream.hpp"
#include "protobuf/protobuf_cpp_infra/ProtoParser.hpp"

TEST(ProtoParserTest, GetVarInt_from_a_single_byte)
{
    infra::ByteInputStream stream(std::array<uint8_t, 1>{ 5 });
    services::ProtoParser parser(stream);

    EXPECT_EQ(5, parser.GetVarInt());
}

TEST(ProtoParserTest, GetVarInt_from_multiple_bytes)
{
    infra::ByteInputStream stream(std::array<uint8_t, 2>{ 0x85, 3 });
    services::ProtoParser parser(stream);

    EXPECT_EQ(389, parser.GetVarInt());
}

TEST(ProtoParserTest, GetFixed32)
{
    infra::ByteInputStream stream(std::array<uint8_t, 4>{ 1, 0, 0, 0 });
    services::ProtoParser parser(stream);

    EXPECT_EQ(1, parser.GetFixed32());
}

TEST(ProtoParserTest, GetFixed64)
{
    infra::ByteInputStream stream(std::array<uint8_t, 8>{ 1, 0, 0, 0, 0, 0, 0, 0 });
    services::ProtoParser parser(stream);

    EXPECT_EQ(1, parser.GetFixed64());
}

TEST(ProtoParserTest, GetField_on_var_int_returns_uint64_t)
{
    infra::ByteInputStream stream(std::array<uint8_t, 2>{ 1 << 3, 5 });
    services::ProtoParser parser(stream);

    services::ProtoParser::Field field = parser.GetField();
    EXPECT_EQ(5, field.first.Get<uint64_t>());
    EXPECT_EQ(1, field.second);
}

TEST(ProtoParserTest, GetField_on_fixed64_returns_uint64_t)
{
    infra::ByteInputStream stream(std::array<uint8_t, 9>{ (1 << 3) | 1, 5, 0, 0, 0, 0, 0, 0, 0 });
    services::ProtoParser parser(stream);

    services::ProtoParser::Field field = parser.GetField();
    EXPECT_EQ(5, field.first.Get<uint64_t>());
    EXPECT_EQ(1, field.second);
}

TEST(ProtoParserTest, GetField_on_fixed32_returns_uint32_t)
{
    infra::ByteInputStream stream(std::array<uint8_t, 5>{ (1 << 3) | 5, 5, 0, 0, 0 });
    services::ProtoParser parser(stream);

    services::ProtoParser::Field field = parser.GetField();
    EXPECT_EQ(5, field.first.Get<uint32_t>());
    EXPECT_EQ(1, field.second);
}

TEST(ProtoParserTest, GetField_returns_string)
{
    infra::ByteInputStream stream(std::array<uint8_t, 3>{ (1 << 3) | 2, 1, 'a' });
    services::ProtoParser parser(stream);

    services::ProtoParser::Field field = parser.GetField();
    infra::BoundedString::WithStorage<10> string;
    field.first.Get<services::ProtoLengthDelimited>().GetString(string);
    EXPECT_EQ("a", string);
}

TEST(ProtoParserTest, GetField_returns_nested_object)
{
    infra::ByteInputStream stream(std::array<uint8_t, 4>{ (1 << 3) | 2, 2, 1 << 3, 5 });
    services::ProtoParser parser(stream);

    services::ProtoParser::Field field = parser.GetField();
    infra::BoundedString::WithStorage<10> string;
    services::ProtoParser nestedParser = field.first.Get<services::ProtoLengthDelimited>().Parser();
    services::ProtoParser::Field nestedField = nestedParser.GetField();
    EXPECT_EQ(5, nestedField.first.Get<uint64_t>());
    EXPECT_EQ(1, nestedField.second);
}
