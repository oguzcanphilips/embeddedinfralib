#include <array>
#include "gmock/gmock.h"
#include "PacketCommunicationBufferedReceive.hpp"

template <int bufferSize = 32>
class BufferedPacketCommunicationFixture
    : public testing::Test
{
public:
    BufferedPacketCommunicationFixture()
        : com(receiveBuffer.data(), receiveBuffer.data() + receiveBuffer.size())
    {}

    class BufferedPacketCommunicationForTest
        : public erpc::PacketCommunicationBufferedReceive
    {
    public:
        BufferedPacketCommunicationForTest(uint8_t* bufferStart, uint8_t* bufferEnd)
            : erpc::PacketCommunicationBufferedReceive(bufferStart, bufferEnd)
            , bufferFullDetected(false)
        {}

        virtual void WriteStartToken() override {}
        virtual void WriteEndToken() override {}
        virtual void WriteByte(uint8_t data) override {}

        virtual void ProcessReceive() override {}

        virtual void BufferFull() override
        {
            bufferFullDetected = true;
        }

        bool bufferFullDetected;
        using erpc::PacketCommunicationBufferedReceive::FindNextMessageStart;
    };

	std::array<uint8_t, bufferSize> receiveBuffer;
    BufferedPacketCommunicationForTest com;
};

typedef BufferedPacketCommunicationFixture<32> BufferedPacketCommunicationFixtureSmallBuffer;
typedef BufferedPacketCommunicationFixture<128000> BufferedPacketCommunicationFixtureLargeBuffer;

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, Construction)
{
    uint8_t data;
    EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, OneBytePacket)
{
    com.ReceivedStart();
    com.ReceivedData(5);
    com.ReceivedEnd();

    uint8_t data;
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(5, data);
    EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureLargeBuffer, VeryLargePacket)
{
	com.ReceivedStart();
	for (int i = 0; i < 127000; ++i)
	{
		com.ReceivedData(i % 255);
	}
	com.ReceivedEnd();

	uint8_t data;
	for (int i = 0; i < 127000; ++i)
	{
		EXPECT_TRUE(com.ReadByte(data));
		EXPECT_EQ(i % 255, data);
	}
	EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, TwoOneBytePackets)
{
    com.ReceivedStart();
    com.ReceivedData(5);
    com.ReceivedEnd();

    com.ReceivedStart();
    com.ReceivedData(8);
    com.ReceivedEnd();

    uint8_t data;
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(5, data);
    EXPECT_FALSE(com.ReadByte(data));
 
    com.FindNextMessageStart();
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(8, data);
    EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, IncompletelyReadPacket)
{
    com.ReceivedStart();
    com.ReceivedData(5);
    com.ReceivedData(6);
    com.ReceivedEnd();

    com.ReceivedStart();
    com.ReceivedData(8);
    com.ReceivedEnd();

    uint8_t data;
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(5, data);
    EXPECT_FALSE(com.IsPacketEnded());
 
    com.FindNextMessageStart();
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(8, data);
    EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, StartWithoutStopOfPrevious)
{
    com.ReceivedStart();
    com.ReceivedData(5);

    com.ReceivedStart();
    com.ReceivedData(8);
    com.ReceivedEnd();

    uint8_t data;
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(8, data);
    EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, DoubleEnd)
{
    com.ReceivedStart();
    com.ReceivedData(5);
    com.ReceivedEnd();
    com.ReceivedEnd();

    com.ReceivedStart();
    com.ReceivedData(8);
    com.ReceivedEnd();

    uint8_t data;
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(5, data);
    EXPECT_FALSE(com.ReadByte(data));
    EXPECT_TRUE(com.IsPacketEnded());
 
    com.FindNextMessageStart();
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(8, data);
    EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, DoubleEndWithData)
{
    com.ReceivedStart();
    com.ReceivedData(5);
    com.ReceivedEnd();
    com.ReceivedData(9);
    com.ReceivedEnd();

    com.ReceivedStart();
    com.ReceivedData(8);
    com.ReceivedEnd();

    uint8_t data;
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(5, data);
    EXPECT_FALSE(com.ReadByte(data));
    EXPECT_TRUE(com.IsPacketEnded());
 
    com.FindNextMessageStart();
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(8, data);
    EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, DoubleDataAfterEnd)
{
    com.ReceivedStart();
    com.ReceivedData(5);
    com.ReceivedEnd();
    com.ReceivedData(9);
    com.ReceivedData(10);

    com.ReceivedStart();
    com.ReceivedData(8);
    com.ReceivedEnd();

    uint8_t data;
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(5, data);
    EXPECT_FALSE(com.ReadByte(data));
    EXPECT_TRUE(com.IsPacketEnded());
 
    com.FindNextMessageStart();
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(8, data);
    EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, CyclicData)
{
    com.ReceivedStart();
    com.ReceivedData(5);
    com.ReceivedEnd();

    com.ReceivedStart();
    com.ReceivedData(8);
    com.ReceivedEnd();

    uint8_t data;
    com.ReadByte(data);
    com.FindNextMessageStart();

    com.ReceivedStart();
    com.ReceivedData(1);
    com.ReceivedData(2);
    com.ReceivedData(3);
    com.ReceivedData(4);
    com.ReceivedEnd();    

    com.ReadByte(data);
    com.FindNextMessageStart();

    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(1, data);
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(2, data);
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(3, data);
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(4, data);
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, OverflowInData)
{
    com.ReceivedStart();

	for (int i = 0; i != receiveBuffer.size() - 5; ++i)
        com.ReceivedData(1);

    EXPECT_FALSE(com.bufferFullDetected);

    com.ReceivedData(1);
    EXPECT_TRUE(com.bufferFullDetected);
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, PacketAfterOverflow)
{
    com.ReceivedStart();
    com.ReceivedData(1);
    com.ReceivedEnd();

    com.ReceivedStart();
	for (int i = 0; i != receiveBuffer.size() - 5 - 4; ++i)
        com.ReceivedData(2);
    com.ReceivedEnd();

    com.ReceivedStart();
    com.ReceivedData(3);
    com.ReceivedEnd();

    uint8_t data;
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(1, data);
    EXPECT_FALSE(com.ReadByte(data));

    com.FindNextMessageStart();
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(3, data);
    EXPECT_FALSE(com.ReadByte(data));
}

TEST_F(BufferedPacketCommunicationFixtureSmallBuffer, OverflowAtStart)
{
    com.ReceivedStart();
    for (int i = 0; i != receiveBuffer.size() - 5; ++i)
        com.ReceivedData(1);
    com.ReceivedEnd();

    com.ReceivedStart();
    EXPECT_TRUE(com.bufferFullDetected);
    com.ReceivedData(2);
    com.ReceivedEnd();

    uint8_t data;
	for (int i = 0; i != receiveBuffer.size() - 5; ++i)
    {
        EXPECT_TRUE(com.ReadByte(data));
        EXPECT_EQ(1, data);
    }

    com.ReceivedStart();
    com.ReceivedData(3);
    com.ReceivedEnd();

    com.FindNextMessageStart();
    EXPECT_TRUE(com.ReadByte(data));
    EXPECT_EQ(3, data);
    EXPECT_FALSE(com.ReadByte(data));
}
