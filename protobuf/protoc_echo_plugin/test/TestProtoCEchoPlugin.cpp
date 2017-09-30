#include "gmock/gmock.h"
#include "generated/echo/TestMessages.pb.hpp"
#include "infra/stream/ByteInputStream.hpp"
#include "infra/stream/ByteOutputStream.hpp"
#include "protobuf/echo/ProtoFormatter.hpp"
#include "protobuf/echo/ProtoParser.hpp"

TEST(ProtoCEchoPluginTest, serialize_string)
{
    test_messages::TestString message;
    message.value = "abcd";

    infra::ByteOutputStream::WithStorage<100> stream;
    services::ProtoFormatter formatter(stream);
    message.Serialize(formatter);

    EXPECT_EQ((std::array<uint8_t, 6>{ 10, 4, 'a', 'b', 'c', 'd' }), stream.Writer().Processed());
}

TEST(ProtoCEchoPluginTest, deserialize_string)
{
    std::array<uint8_t, 6> data{ 10, 4, 'a', 'b', 'c', 'd' };
    infra::ByteInputStream stream(data);
    services::ProtoParser parser(stream);

    test_messages::TestString message(parser);
    EXPECT_EQ("abcd", message.value);
}

TEST(ProtoCEchoPluginTest, serialize_repeated_string)
{
    test_messages::TestRepeatedString message;
    message.value.push_back("abcd");
    message.value.push_back("ef");

    infra::ByteOutputStream::WithStorage<100> stream;
    services::ProtoFormatter formatter(stream);
    message.Serialize(formatter);

    EXPECT_EQ((std::array<uint8_t, 10>{ 10, 4, 'a', 'b', 'c', 'd', 10, 2, 'e', 'f' }), stream.Writer().Processed());
}

TEST(ProtoCEchoPluginTest, deserialize_repeated_string)
{
    std::array<uint8_t, 10> data{ 10, 4, 'a', 'b', 'c', 'd', 10, 2, 'e', 'f' };
    infra::ByteInputStream stream(data);
    services::ProtoParser parser(stream);

    test_messages::TestRepeatedString message(parser);
    infra::BoundedVector<infra::BoundedString::WithStorage<20>>::WithMaxSize<20> expected;
    expected.push_back("abcd");
    expected.push_back("ef");
    EXPECT_EQ(expected, message.value);
}

TEST(ProtoCEchoPluginTest, serialize_uint32)
{
    test_messages::TestUint32 message;
    message.value = 5;

    infra::ByteOutputStream::WithStorage<100> stream;
    services::ProtoFormatter formatter(stream);
    message.Serialize(formatter);

    EXPECT_EQ((std::array<uint8_t, 2>{ 1 << 3, 5 }), stream.Writer().Processed());
}

TEST(ProtoCEchoPluginTest, deserialize_uint32)
{
    std::array<uint8_t, 2> data{ 1 << 3, 5 };
    infra::ByteInputStream stream(data);
    services::ProtoParser parser(stream);

    test_messages::TestUint32 message(parser);
    EXPECT_EQ(5, message.value);
}

TEST(ProtoCEchoPluginTest, serialize_message)
{
    test_messages::TestMessageWithMessageField message;
    message.message.value = 5;

    infra::ByteOutputStream::WithStorage<100> stream;
    services::ProtoFormatter formatter(stream);
    message.Serialize(formatter);

    EXPECT_EQ((std::array<uint8_t, 4>{ (1 << 3) | 2, 2, 1 << 3, 5 }), stream.Writer().Processed());
}

TEST(ProtoCEchoPluginTest, deserialize_message)
{
    std::array<uint8_t, 4> data{ (1 << 3) | 2, 2, 1 << 3, 5 };
    infra::ByteInputStream stream(data);
    services::ProtoParser parser(stream);

    test_messages::TestMessageWithMessageField message(parser);
    EXPECT_EQ(5, message.message.value);
}

TEST(ProtoCEchoPluginTest, serialize_nested_message)
{
    test_messages::TestNestedMessage message;
    message.message.value = 5;

    infra::ByteOutputStream::WithStorage<100> stream;
    services::ProtoFormatter formatter(stream);
    message.Serialize(formatter);

    EXPECT_EQ((std::array<uint8_t, 4>{ (1 << 3) | 2, 2, 1 << 3, 5 }), stream.Writer().Processed());
}

TEST(ProtoCEchoPluginTest, deserialize_nested_message)
{
    std::array<uint8_t, 4> data{ (1 << 3) | 2, 2, 1 << 3, 5 };
    infra::ByteInputStream stream(data);
    services::ProtoParser parser(stream);

    test_messages::TestNestedMessage message(parser);
    EXPECT_EQ(5, message.message.value);
}

TEST(ProtoCEchoPluginTest, serialize_nested_repeated_message)
{
    test_messages::TestNestedRepeatedMessage message;
    message.message.push_back(test_messages::TestNestedRepeatedMessage::NestedMessage());
    message.message[0].value = 5;

    infra::ByteOutputStream::WithStorage<100> stream;
    services::ProtoFormatter formatter(stream);
    message.Serialize(formatter);

    EXPECT_EQ((std::array<uint8_t, 4>{ (1 << 3) | 2, 2, 1 << 3, 5 }), stream.Writer().Processed());
}

TEST(ProtoCEchoPluginTest, deserialize_nested_repeated_message)
{
    std::array<uint8_t, 4> data{ (1 << 3) | 2, 2, 1 << 3, 5 };
    infra::ByteInputStream stream(data);
    services::ProtoParser parser(stream);

    test_messages::TestNestedRepeatedMessage message(parser);
    EXPECT_EQ(5, message.message[0].value);
}
