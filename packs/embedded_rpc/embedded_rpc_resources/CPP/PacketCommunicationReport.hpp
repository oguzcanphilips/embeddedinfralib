#ifndef PACKETCOMMUNICATIONREPORT_HPP
#define PACKETCOMMUNICATIONREPORT_HPP

#include "PacketCommunicationBin.hpp"

namespace erpc
{
    class IReportIO
    {
    public:
        virtual void Send(uint8_t* v, uint16_t len) = 0;
        virtual bool Receive(uint8_t* v) = 0;
    };

    class PacketCommunicationReportBase : public PacketCommunicationBin
    {
    public:
        void ProcessReceive();
        bool IsPacketEnded();
        void WriteStartToken();
        void WriteEndToken();
        void WriteByte(uint8_t data);
        bool ReadByte(uint8_t& v);
    protected:
        PacketCommunicationReportBase(IReportIO& reportIO, uint8_t* wrBuffer, uint8_t* rdBuffer, uint16_t bufferSize);
    private:
        void SendReport();
        bool ReceiveReport();
        bool IsFirstReadReport();
        bool IsLastReadReport();
        uint16_t mHeaderSize;
        uint16_t mWr;
        uint16_t mRd;
        uint16_t mReadRemain;
        uint16_t mBufferSize;
        uint8_t* mWrBuffer;
        uint8_t* mRdBuffer;
        IReportIO& mReportIO;
    };

    template<uint16_t R_LEN>
    class PacketCommunicationReport : public PacketCommunicationReportBase
    {
    public:
        PacketCommunicationReport(IReportIO& reportIO)
            : PacketCommunicationReportBase(reportIO, mWrBuffer, mRdBuffer, R_LEN)
        {
        }
    private:
        uint8_t mWrBuffer[R_LEN];
        uint8_t mRdBuffer[R_LEN];
    };
}
#endif