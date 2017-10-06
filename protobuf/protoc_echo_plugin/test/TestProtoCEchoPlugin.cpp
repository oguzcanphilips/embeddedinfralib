#include "gmock/gmock.h"
#include "generated/echo/TestMessages.pb.hpp"
#include "infra/stream/ByteInputStream.hpp"
#include "infra/stream/ByteOutputStream.hpp"
#include "infra/util/test_helper/MockCallback.hpp"
#include "protobuf/echo/ProtoFormatter.hpp"
#include "protobuf/echo/ProtoParser.hpp"
#include "services/network/test_doubles/ConnectionMock.hpp"

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

TEST(ProtoCEchoPluginTest, serialize_repeated_uint32)
{
    test_messages::TestRepeatedUint32 message;
    message.value.push_back(5);
    message.value.push_back(6);

    infra::ByteOutputStream::WithStorage<100> stream;
    services::ProtoFormatter formatter(stream);
    message.Serialize(formatter);

    EXPECT_EQ((std::array<uint8_t, 4>{ 1 << 3, 5, 1 << 3, 6 }), stream.Writer().Processed());
}

TEST(ProtoCEchoPluginTest, deserialize_repeated_uint32)
{
    std::array<uint8_t, 4> data{ 1 << 3, 5, 1 << 3, 6 };
    infra::ByteInputStream stream(data);
    services::ProtoParser parser(stream);

    test_messages::TestRepeatedUint32 message(parser);
    EXPECT_EQ(2, message.value.size());
    EXPECT_EQ(5, message.value[0]);
    EXPECT_EQ(6, message.value[1]);
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

TEST(ProtoCEchoPluginTest, invoke_service_proxy_method)
{
    services::ConnectionMock connection;
    services::Echo echo(connection);
    test_messages::TestService1Proxy service(echo);

    testing::StrictMock<infra::MockCallback<void()>> onGranted;
    EXPECT_CALL(connection, RequestSendStream(8));
    service.RequestSend([&onGranted]() { onGranted.callback(); });

    infra::ByteOutputStream::WithStorage<128> stream;
    auto streamPtr = infra::UnOwnedSharedPtr(stream);
    EXPECT_CALL(onGranted, callback());
    connection.GetObserver().SendStreamAvailable(streamPtr);

    service.Method(test_messages::TestUint32(5));
    EXPECT_EQ((std::vector<uint8_t>{ 1, 10, 2, 8, 5 }), (std::vector<uint8_t>(stream.Storage().begin(), stream.Storage().begin() + 5)));
}

class TestService1Mock
    : public test_messages::TestService1
{
public:
    using test_messages::TestService1::TestService1;

    MOCK_METHOD1(Method, void(const test_messages::TestUint32& argument));
};

TEST(ProtoCEchoPluginTest, service_method_is_invoked)
{
    testing::StrictMock<services::ConnectionMock> connection;
    services::Echo echo(connection);
    testing::StrictMock<TestService1Mock> service(echo);

    infra::ByteInputStream::WithStorage<128> stream;
    infra::Copy(infra::MakeRange(std::array<uint8_t, 5>{ 1, 10, 2, 8, 5 }), infra::Head(infra::MakeRange(stream.Storage()), 5));
    auto streamPtr = infra::UnOwnedSharedPtr(stream);
    EXPECT_CALL(connection, ReceiveStream()).WillOnce(testing::Return(streamPtr));
    EXPECT_CALL(service, Method(test_messages::TestUint32(5)));
    EXPECT_CALL(connection, AckReceived());
    connection.GetObserver().DataReceived();
}
