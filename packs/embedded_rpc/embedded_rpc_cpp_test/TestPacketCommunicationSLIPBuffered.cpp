#include <array>
#include <iterator>
#include "gmock/gmock.h"
#include "PacketCommunicationSLIPBuffered.hpp"
#include "PacketCommunicationDefines.h"
#include "CommCallbackMock.h"

class PacketCommunicationSLIPBufferedReceiveFixture
    : public testing::Test
{
public:
    PacketCommunicationSLIPBufferedReceiveFixture()
        : com()
    {}

    class PacketCommunicationSLIPBufferedReceiveForTest
        : public erpc::PacketCommunicationSLIPBufferedReceive
    {
    public:
        PacketCommunicationSLIPBufferedReceiveForTest()
            : erpc::PacketCommunicationSLIPBufferedReceive(receiveBuffer.data(), receiveBuffer.data() + receiveBuffer.size())
            , receiveBuffer()
        {}

        virtual void WriteSlipByte(uint8_t data) override
        {
            std::abort();
        }

        std::array<uint8_t, 256> receiveBuffer;
    };

    PacketCommunicationSLIPBufferedReceiveForTest com;
};

TEST_F(PacketCommunicationSLIPBufferedReceiveFixture, Creation)
{
    EXPECT_EQ(0, com.receiveBuffer[0]);
}

TEST_F(PacketCommunicationSLIPBufferedReceiveFixture, ReceiveOneByte)
{
	CommCallbackMock callbackMock(com, 0, 1);

    com.ReceivedSlipByte(SLIP_BEGIN);
    com.ReceivedSlipByte(0);
	com.ReceivedSlipByte(1);
    com.ReceivedSlipByte(SLIP_END);

	EXPECT_EQ(1, callbackMock.data[0]);
}

TEST_F(PacketCommunicationSLIPBufferedReceiveFixture, ReceiveTwoBytes)
{
	CommCallbackMock callbackMock(com, 0, 2);

    com.ReceivedSlipByte(SLIP_BEGIN);
    com.ReceivedSlipByte(0);
	com.ReceivedSlipByte(3);
    com.ReceivedSlipByte(4);
    com.ReceivedSlipByte(SLIP_END);

	EXPECT_EQ(3, callbackMock.data[0]);
	EXPECT_EQ(4, callbackMock.data[1]);
}

TEST_F(PacketCommunicationSLIPBufferedReceiveFixture, ReceiveEscapedBegin)
{
	CommCallbackMock callbackMock(com, 0, 1);

    com.ReceivedSlipByte(SLIP_BEGIN);
	com.ReceivedSlipByte(0);
    com.ReceivedSlipByte(SLIP_ESC);
    com.ReceivedSlipByte(SLIP_ESC_BEGIN);
    com.ReceivedSlipByte(SLIP_END);

	EXPECT_EQ(SLIP_BEGIN, callbackMock.data[0]);
}

TEST_F(PacketCommunicationSLIPBufferedReceiveFixture, ReceiveEscapedEnd)
{
	CommCallbackMock callbackMock(com, 0, 1);

    com.ReceivedSlipByte(SLIP_BEGIN);
	com.ReceivedSlipByte(0);
    com.ReceivedSlipByte(SLIP_ESC);
    com.ReceivedSlipByte(SLIP_ESC_END);
    com.ReceivedSlipByte(SLIP_END);

	EXPECT_EQ(SLIP_END, callbackMock.data[0]);
}

TEST_F(PacketCommunicationSLIPBufferedReceiveFixture, ReceiveEscapedEscape)
{
	CommCallbackMock callbackMock(com, 0, 1);

    com.ReceivedSlipByte(SLIP_BEGIN);
	com.ReceivedSlipByte(0);
    com.ReceivedSlipByte(SLIP_ESC);
    com.ReceivedSlipByte(SLIP_ESC_ESC);
    com.ReceivedSlipByte(SLIP_END);

	EXPECT_EQ(SLIP_ESC, callbackMock.data[0]);
}

class PacketCommunicationSLIPBufferedFixture
    : public testing::Test
{
public:
    PacketCommunicationSLIPBufferedFixture()
        : com()
    {}

    class PacketCommunicationSLIPBufferedForTest
        : public erpc::PacketCommunicationSLIPBuffered
    {
    public:
        PacketCommunicationSLIPBufferedForTest()
            : erpc::PacketCommunicationSLIPBuffered(sendBuffer.data(), sendBuffer.data() + sendBuffer.size(), receiveBuffer.data(), receiveBuffer.data() + receiveBuffer.size())
            , sendBuffer()
            , receiveBuffer()
            , bufferOverflowDetected(false)
        {}

        using PacketCommunicationSLIPBuffered::WriteSlipByte;

        void SendDone()
        {
            PacketCommunicationSLIPBuffered::SendDone(sendEnd);
        }

    protected:
        void StartSending(uint8_t* start, uint8_t* end) override
        {
            sent.push_back(std::vector<uint8_t>(start, end));
            sendEnd = end;
        }

        void BufferOverflow()
        {
            bufferOverflowDetected = true;
        }

    public:
        std::vector<std::vector<uint8_t>> sent;
        bool bufferOverflowDetected;

    private:
        std::array<uint8_t, 4> sendBuffer;
        std::array<uint8_t, 1> receiveBuffer;

        uint8_t* sendStart;
        uint8_t* sendEnd;
    };

    PacketCommunicationSLIPBufferedForTest com;
};

TEST_F(PacketCommunicationSLIPBufferedFixture, Construction)
{}

TEST_F(PacketCommunicationSLIPBufferedFixture, SendOneByte)
{
    com.WriteSlipByte(1);

    EXPECT_EQ(std::vector<std::vector<uint8_t>>{{ 1 }}, com.sent);
}

TEST_F(PacketCommunicationSLIPBufferedFixture, SendThreeBytes)
{
    com.WriteSlipByte(1);
    com.WriteSlipByte(2);
    com.WriteSlipByte(3);
    com.SendDone();

    EXPECT_EQ((std::vector<std::vector<uint8_t>>{{ 1 }, { 2, 3 }}), com.sent);
}

TEST_F(PacketCommunicationSLIPBufferedFixture, SendAcrossCycleBoundary)
{
    com.WriteSlipByte(1);
    com.SendDone();
    com.WriteSlipByte(2);
    com.SendDone();
    com.WriteSlipByte(3);
    com.WriteSlipByte(4);
    com.WriteSlipByte(5);
    com.SendDone();
    com.SendDone();

    EXPECT_EQ((std::vector<std::vector<uint8_t>>{{ 1 }, { 2 }, { 3 }, { 4 }, { 5 }}), com.sent);
}

TEST_F(PacketCommunicationSLIPBufferedFixture, OverflowBuffer)
{
    com.WriteSlipByte(1);
    com.WriteSlipByte(2);
    com.WriteSlipByte(3);
    com.WriteSlipByte(4);

    EXPECT_TRUE(com.bufferOverflowDetected);
}
