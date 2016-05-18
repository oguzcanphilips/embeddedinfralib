#include "PacketCommunicationReportBuffered.hpp"
#include "../PacketCommunicationDefines.h"

namespace erpc
{
    PacketCommunicationReportBuffered::PacketCommunicationReportBuffered(
        uint8_t* receiveBufferStart, uint8_t* receiveBufferEnd,
        uint8_t* sendBuffer, uint16_t reportSize)
        : PacketCommunicationBufferedReceive(receiveBufferStart, receiveBufferEnd)
        , sendIndex(0)
        , sendBuffer(sendBuffer)
        , reportSize(reportSize)
    {
    }

    void PacketCommunicationReportBuffered::WriteStartToken()
    {
        sendIndex = REPORT_HEADER_SIZE(reportSize);
        sendBuffer[0] = IS_FIRST_REPORT_MASK;
    }

    void PacketCommunicationReportBuffered::WriteEndToken()
    {
        sendBuffer[0] |= IS_LAST_REPORT_MASK;
        SendReport();
    }
    void PacketCommunicationReportBuffered::SendReport()
    {
        if (REPORT_HEADER_SIZE(reportSize) == 2)
        {
            sendBuffer[0] |= (sendIndex - 2) >> 8;
            sendBuffer[1] = (sendIndex - 2) & 0xff;
        }
        else
        {
            sendBuffer[0] |= (sendIndex - 1);
        }
        WriteReport(sendBuffer, sendIndex);
    }

    void PacketCommunicationReportBuffered::WriteByte(uint8_t data)
    {
        if (sendIndex == reportSize)
        {
            SendReport();
            sendIndex = REPORT_HEADER_SIZE(reportSize);
            sendBuffer[0] = 0;
        }
        sendBuffer[sendIndex] = data;
        sendIndex++;
    }

    void PacketCommunicationReportBuffered::ReceivedReport(const uint8_t* data, uint16_t /*lenOfData*/)
    {
        if (data[0] & IS_FIRST_REPORT_MASK)
            ReceivedStart();

        uint16_t len = data[0] & REPORT_SIZE_MASK;
        if (REPORT_HEADER_SIZE(reportSize) == 2)
            len = (len << 8) + data[1];

        const uint8_t* payload = &data[REPORT_HEADER_SIZE(reportSize)];
        for (uint16_t i = 0; i != len; ++i)
            ReceivedData(payload[i]);

        if (data[0] & IS_LAST_REPORT_MASK)
            ReceivedEnd();
    }
}