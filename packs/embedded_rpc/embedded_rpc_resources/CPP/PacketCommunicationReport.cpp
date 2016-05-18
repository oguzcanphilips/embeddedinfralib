#include "PacketCommunicationReport.hpp"
#include "../PacketCommunicationDefines.h"

namespace erpc
{
    PacketCommunicationReportBase::PacketCommunicationReportBase(IReportIO& reportIO, uint8_t* wrBuffer, uint8_t* rdBuffer, uint16_t bufferSize)
        : mHeaderSize(REPORT_HEADER_SIZE(bufferSize))
        , mWr(0)
        , mRd(0)
        , mReadRemain(0)
        , mBufferSize(bufferSize)
        , mWrBuffer(wrBuffer)
        , mRdBuffer(rdBuffer)
        , mReportIO(reportIO)
    {}

    void PacketCommunicationReportBase::ProcessReceive()
    {
        if (ReceiveReport() && IsFirstReadReport())
        {
            Receive();
        }
    }

    bool PacketCommunicationReportBase::IsPacketEnded()
    {
        return IsLastReadReport() && (mReadRemain == 0);
    }

    void PacketCommunicationReportBase::WriteEndToken()
    {
        mWrBuffer[0] |= IS_LAST_REPORT_MASK;
        SendReport();
    }

    void PacketCommunicationReportBase::WriteByte(uint8_t data)
    {
        if (mWr == mBufferSize)
        {
            SendReport();
        }
        mWrBuffer[mWr++] = data;
    }

    void PacketCommunicationReportBase::SendReport()
    {
        uint16_t dataLen = mWr - mHeaderSize;
        if (mHeaderSize == 2)
        {
            mWrBuffer[1] = dataLen & 0xff;
            dataLen >>= 8;
        }
        mWrBuffer[0] |= REPORT_SIZE_MASK & dataLen;
        mReportIO.Send(mWrBuffer, mWr);
        mWrBuffer[0] = 0;
        mWr = mHeaderSize;
    }

    bool PacketCommunicationReportBase::ReceiveReport()
    {
        mRd = mHeaderSize;
        if (!mReportIO.Receive(mRdBuffer)) return false;
        mReadRemain = mRdBuffer[0] & REPORT_SIZE_MASK;
        if (mHeaderSize == 2)
        {
            mReadRemain <<= 8;
            mReadRemain += mRdBuffer[1];
        }
        if (mReadRemain > (mBufferSize - mHeaderSize))
        {
            mReadRemain = 0;
            return false;
        }
        return true;
    }

    bool PacketCommunicationReportBase::ReadByte(uint8_t& v)
    {
        if (mRd == mBufferSize)
        {
            if (IsLastReadReport() ||
                !ReceiveReport() ||
                IsFirstReadReport()) return false;
        }
        v = mRdBuffer[mRd++];
        if (mReadRemain == 0)
        {
            mRd = mBufferSize;
            return false;
        }
        mReadRemain--;
        return true;
    }

    void PacketCommunicationReportBase::WriteStartToken()
    {
        mWrBuffer[0] = IS_FIRST_REPORT_MASK;
        mWr = mHeaderSize;
    }

    bool PacketCommunicationReportBase::IsFirstReadReport()
    {
        return (mRdBuffer[0] & IS_FIRST_REPORT_MASK) == IS_FIRST_REPORT_MASK;
    }

    bool PacketCommunicationReportBase::IsLastReadReport()
    {
        return (mRdBuffer[0] & IS_LAST_REPORT_MASK) == IS_LAST_REPORT_MASK;
    }
}