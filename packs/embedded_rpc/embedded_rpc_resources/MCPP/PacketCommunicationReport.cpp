#include "PacketCommunicationReport.hpp"
#include "../PacketCommunicationDefines.h"

PacketCommunicationReport::PacketCommunicationReport(IReportIO^ reportIO)
: mReportIO(reportIO)
, mHeaderSize(REPORT_HEADER_SIZE(reportIO->GetReportLength()))
, mRd(0)
, mWr(0)
, mReadRemain(0)
, mRdBuffer(gcnew array<uint8_t>(reportIO->GetReportLength()))
, mWrBuffer(gcnew array<uint8_t>(reportIO->GetReportLength()))
{
    Start();
}

PacketCommunicationReport::~PacketCommunicationReport()
{
    Stop();
}

void PacketCommunicationReport::ProcessReceive()
{
    if(ReceiveReport() && IsFirstReadReport())
    {
        Receive();
    }
}

bool PacketCommunicationReport::IsPacketEnded()
{
    return IsLastReadReport() && (mReadRemain == 0);
}

void PacketCommunicationReport::PacketStart()
{
    mWrBuffer[0] = IS_FIRST_REPORT_MASK;
    mWr = mHeaderSize;
}

void PacketCommunicationReport::PacketEnd()
{
    mWrBuffer[0] |= IS_LAST_REPORT_MASK;
    SendReport();
}

void PacketCommunicationReport::WriteByte(uint8_t v)
{
    if(mWr == mWrBuffer->Length)
    {
        SendReport();
    }
    mWrBuffer[mWr++] = v;
}

bool PacketCommunicationReport::ReadByte(uint8_t% v)
{
    if(mRd == mRdBuffer->Length)
    {
        if(IsLastReadReport() ||
           !ReceiveReport()   ||
           IsFirstReadReport() ) return false;        
    }
    v = mRdBuffer[mRd++];
    if(mReadRemain == 0)
    {
		mRd = (uint16_t)(mRdBuffer->Length);
        return false;
    }
    mReadRemain--;
    return true;
}

void PacketCommunicationReport::SendReport()
{
    uint16_t dataLen = mWr - mHeaderSize;
    if(mHeaderSize==2)
    {
        mWrBuffer[1] = dataLen & 0xff;
        dataLen >>= 8;
    }
    mWrBuffer[0] |= REPORT_SIZE_MASK & dataLen;
    mReportIO->Send(mWrBuffer, mWr);
    mWrBuffer[0] = 0;
    mWr = mHeaderSize;
}

bool PacketCommunicationReport::ReceiveReport()
{
    mRd=mHeaderSize;
    if(!mReportIO->Receive(mRdBuffer)) return false;
    mReadRemain = mRdBuffer[0] & REPORT_SIZE_MASK;
    if(mHeaderSize == 2)
    {
        mReadRemain <<=8;
        mReadRemain += mRdBuffer[1];
    }
    if(mReadRemain > (mReportIO->GetReportLength() - mHeaderSize))
    {
        mReadRemain = 0;
        return false;
    }
    return true;  
}

bool PacketCommunicationReport::IsFirstReadReport()
{
    return (mRdBuffer[0] & IS_FIRST_REPORT_MASK)==IS_FIRST_REPORT_MASK;
}

bool PacketCommunicationReport::IsLastReadReport()
{
    return (mRdBuffer[0] & IS_LAST_REPORT_MASK)==IS_LAST_REPORT_MASK;
}
