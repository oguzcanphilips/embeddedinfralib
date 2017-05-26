#include "gmock/gmock.h"
#include "hal/interfaces/test_doubles/SerialCommunicationMock.hpp"
#include "services/tracer/StreamWriterOnSerialCommunication.hpp"

class StreamWriterOnSerialCommunicationTest
    : public testing::Test
{
public:
    StreamWriterOnSerialCommunicationTest()
        : streamWriter(communication)
    {}

    testing::StrictMock<hal::SerialCommunicationMock> communication;
    services::StreamWriterOnSerialCommunication::WithStorage<4> streamWriter;
};

TEST_F(StreamWriterOnSerialCommunicationTest, Insert_sends_one_element_on_communication)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 5 }));
    streamWriter.Insert(5);
}

TEST_F(StreamWriterOnSerialCommunicationTest, no_communication_when_buffer_is_empty)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 5 }));
    streamWriter.Insert(5);
    communication.actionOnCompletion();
}

TEST_F(StreamWriterOnSerialCommunicationTest, Insert_sends_range_on_communication)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 5, 8 }));
    streamWriter.Insert(std::array<uint8_t, 2>{ 5, 8 });
}

TEST_F(StreamWriterOnSerialCommunicationTest, second_Insert_holds_if_communication_not_complete)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 5 }));
    streamWriter.Insert(5);
    streamWriter.Insert(8);
}

TEST_F(StreamWriterOnSerialCommunicationTest, second_Insert_is_sent_after_communication_is_done)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 5 }));
    streamWriter.Insert(5);
    streamWriter.Insert(8);

    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 8 }));
    communication.actionOnCompletion();
}

TEST_F(StreamWriterOnSerialCommunicationTest, on_overflow_rest_is_discarded)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 1, 2, 3, 4 }));
    streamWriter.Insert(std::vector<uint8_t>{ 1, 2, 3, 4, 5 });
}
