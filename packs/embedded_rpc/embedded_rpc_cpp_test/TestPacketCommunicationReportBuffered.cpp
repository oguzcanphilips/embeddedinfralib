#include "gtest\gtest.h"
#include "PacketCommunicationReportBuffered.hpp"
#include "../PacketCommunicationDefines.h"
#include "CommCallbackMock.h"
#include <list>
#include <vector>
#include <array>

class LocalReport : public erpc::PacketCommunicationReportBuffered
{
public:
	const static uint16_t reportSize = 19;
	std::array<uint8_t, 1000> receiveBuffer;
	std::array<uint8_t, reportSize> sendBuffer;

    LocalReport() : erpc::PacketCommunicationReportBuffered(receiveBuffer.data(), receiveBuffer.data() + receiveBuffer.size(), sendBuffer.data(), sendBuffer.size())
	{

	}
	void ReceivedReport(uint8_t* data, uint16_t lenOfData)
	{
        erpc::PacketCommunicationReportBuffered::ReceivedReport(data, lenOfData);
	}
	void WriteReport(uint8_t* data, uint16_t lenOfData) override
	{
		writtenReports.push_back(std::vector<uint8_t>(data, data + lenOfData));
	}
    void ProcessReceivedPackets()
    {
        erpc::PacketCommunicationReportBuffered::ProcessReceive();
    }
    void ProcessReceive()
    {
        // dummy to prevent processing per received packet.
    }
	std::list<std::vector<uint8_t>> writtenReports;
};

class PacketCommunicationReportBufferedFixture
    : public testing::Test
{
public:
	PacketCommunicationReportBufferedFixture()
    {}	
protected:
	LocalReport packetCommReport;
};

TEST_F(PacketCommunicationReportBufferedFixture, SendMessage)
{
	CommCallbackMock callback(packetCommReport, 0, 2);

    packetCommReport.PacketStartToken();
    for (uint8_t i = 0; i < 25; ++i)
    {
        packetCommReport.WriteByte(i);
    }
    packetCommReport.PackedEndToken();
    ASSERT_EQ(2, packetCommReport.writtenReports.size());
    
    std::vector<uint8_t> report1 = packetCommReport.writtenReports.front();
    packetCommReport.writtenReports.pop_front();
    std::vector<uint8_t> report2 = packetCommReport.writtenReports.front();

    EXPECT_EQ(0x80 + 18, report1[0]);
    EXPECT_EQ(0x40 + 7, report2[0]);

    for (uint8_t i = 0; i < 18; ++i)
    {
        EXPECT_EQ(i, report1[i + 1]);
    }
    for (uint8_t i = 0; i < 7; ++i)
    {
        EXPECT_EQ(i + 18, report2[i + 1]);
    }
}

TEST_F(PacketCommunicationReportBufferedFixture, SendReport)
{
	CommCallbackMock callback(packetCommReport, 0,2);

	uint8_t report[LocalReport::reportSize];

    uint8_t payloadLen = 3;
    report[0] = IS_FIRST_REPORT_MASK | IS_LAST_REPORT_MASK + payloadLen;

    // interface Id
	report[1] = 0; 
    // payload for callback
	report[2] = 1;
	report[3] = 2;

	packetCommReport.ReceivedReport(report, 4);
    packetCommReport.ProcessReceivedPackets();
	ASSERT_EQ(1, callback.data[0]);
	ASSERT_EQ(2, callback.data[1]);
}

TEST_F(PacketCommunicationReportBufferedFixture, SendReports)
{
	CommCallbackMock callback0(packetCommReport, 0, 2);
	CommCallbackMock callback1(packetCommReport, 1, 2);

    uint8_t report[LocalReport::reportSize];

    uint8_t payloadLen = 3;
    report[0] = IS_FIRST_REPORT_MASK | IS_LAST_REPORT_MASK + payloadLen;

    // interface Id
    report[1] = 0;
    // payload for callback
    report[2] = 1;
    report[3] = 2;
    packetCommReport.ReceivedReport(report, 4);

    // interface Id
    report[1] = 1;
    // payload for callback
    report[2] = 3;
    report[3] = 4;
    packetCommReport.ReceivedReport(report, 4);

    packetCommReport.ProcessReceivedPackets();

    ASSERT_EQ(1, callback0.data[0]);
    ASSERT_EQ(2, callback0.data[1]);

    ASSERT_EQ(3, callback1.data[0]);
    ASSERT_EQ(4, callback1.data[1]);
}
