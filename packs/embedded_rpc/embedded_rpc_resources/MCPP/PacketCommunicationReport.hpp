#ifndef PACKETCOMMUNICATIONREPORT_H
#define PACKETCOMMUNICATIONREPORT_H

#include "PacketCommunication.h"
using namespace System;


public ref class PacketCommunicationReport : public PacketCommunication
{
public:
    interface class IReportIO
    {
    public:
        void Open();
        bool IsOpen();
        void Close();
        void Send(array<uint8_t>^ v, uint16_t len);
        bool Receive(array<uint8_t>^% v);

        uint16_t GetReportLength();
    };
    PacketCommunicationReport(IReportIO^ reportIO);
    ~PacketCommunicationReport();
    virtual void ProcessReceive() override;
    virtual bool IsPacketEnded() override;
    virtual void PacketStart() override;
    virtual void PacketEnd() override;
    virtual void WriteByte(uint8_t v) override;
    virtual bool ReadByte(uint8_t% v) override;
private:
    void SendReport();
    bool ReceiveReport();
    bool IsFirstReadReport();
    bool IsLastReadReport();
    uint16_t mHeaderSize;
    uint16_t mRd;
    uint16_t mWr;
    uint16_t mReadRemain;
    array<uint8_t>^ mRdBuffer;
    array<uint8_t>^ mWrBuffer;
    IReportIO^ mReportIO;
};


#endif