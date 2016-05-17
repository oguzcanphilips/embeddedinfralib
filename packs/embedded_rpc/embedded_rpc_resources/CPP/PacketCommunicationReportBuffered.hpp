#ifndef PACKET_COMMUNICATION_REPORT_BUFFERED_HPP
#define PACKET_COMMUNICATION_REPORT_BUFFERED_HPP

#include "PacketCommunicationBufferedReceive.hpp"

namespace erpc
{
    class PacketCommunicationReportBuffered : public PacketCommunicationBufferedReceive
    {
    public:
        PacketCommunicationReportBuffered(
            uint8_t* receiveBufferStart, uint8_t* receiveBufferEnd,
            uint8_t* sendBuffer, uint16_t reportSize);

        virtual void PacketStartToken();
        virtual void PackedEndToken();
        virtual void WriteByte(uint8_t data);

    protected:
        void ReceivedReport(const uint8_t* data, uint16_t lenOfData);
        virtual void WriteReport(uint8_t* data, uint16_t lenOfData) = 0;
    private:
        void SendReport();

        uint16_t sendIndex;
        uint8_t* sendBuffer;
        const uint16_t reportSize;
    };
}
#endif
