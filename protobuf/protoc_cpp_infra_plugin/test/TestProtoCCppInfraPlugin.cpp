#include "gmock/gmock.h"
#include "generated/proto_cpp_infra/TestMessages.pb.hpp"
#include "infra/stream/ByteInputStream.hpp"
#include "infra/stream/ByteOutputStream.hpp"
#include "protobuf/protobuf_cpp_infra/ProtoFormatter.hpp"
#include "protobuf/protobuf_cpp_infra/ProtoParser.hpp"

TEST(ProtoCCppInfraPluginTest, serialize_string)
{
    test_messages::TestString message;
    message.value = "abcd";

    infra::ByteOutputStream::WithStorage<100> stream;
    services::ProtoFormatter formatter(stream);
    message.Serialize(formatter);

    EXPECT_EQ((std::array<uint8_t, 6>{ 10, 4, 'a', 'b', 'c', 'd' }), stream.Writer().Processed());
}

TEST(ProtoCCppInfraPluginTest, deserialize_string)
{
    std::array<uint8_t, 6> data{ 10, 4, 'a', 'b', 'c', 'd' };
    infra::ByteInputStream stream(data);
    services::ProtoParser parser(stream);

    test_messages::TestString message(parser);
    EXPECT_EQ("abcd", message.value);
}

TEST(ProtoCCppInfraPluginTest, serialize_repeated_string)
{
    test_messages::TestRepeatedString message;
    message.value.push_back("abcd");
    message.value.push_back("ef");

    infra::ByteOutputStream::WithStorage<100> stream;
    services::ProtoFormatter formatter(stream);
    message.Serialize(formatter);

    EXPECT_EQ((std::array<uint8_t, 10>{ 10, 4, 'a', 'b', 'c', 'd', 10, 2, 'e', 'f' }), stream.Writer().Processed());
}

TEST(ProtoCCppInfraPluginTest, deserialize_repeated_string)
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
