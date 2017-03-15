#include "gmock/gmock.h"
#include "hal/synchronous_interfaces/test_doubles/public/SynchronousSerialCommunicationMock.hpp"
#include "services/tracer/public/StreamWriterOnSynchronousSerialCommunication.hpp"

class StreamWriterOnSynchronousSerialCommunicationTest
    : public testing::Test
{
public:
    StreamWriterOnSynchronousSerialCommunicationTest()
        : streamWriter(communication)
    {}

    testing::StrictMock<hal::SynchronousSerialCommunicationMock> communication;
    services::StreamWriterOnSynchronousSerialCommunication streamWriter;
};

TEST_F(StreamWriterOnSynchronousSerialCommunicationTest, Insert_sends_one_element_on_communication)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 5 }));
    streamWriter.Insert(5);
}

TEST_F(StreamWriterOnSynchronousSerialCommunicationTest, Insert_sends_range_on_communication)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 5, 8 }));
    streamWriter.Insert(std::array<uint8_t, 2>{ 5, 8 });
}
