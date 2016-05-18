#include "PacketCommunicationReport.h"
#include "../PacketCommunicationDefines.h"
#include <string.h>

static void SendReport(PacketCommunicationReport_t* me)
{
    uint16_t dataLen = me->mWr - me->mHeaderSize;
    if(me->mHeaderSize==2)
    {
        me->mWrBuffer[1] = dataLen & 0xff;
        dataLen >>= 8;
    }
    me->mWrBuffer[0] |= REPORT_SIZE_MASK & dataLen;
    me->ReportSend(me->mWrBuffer, me->mWr);
    me->mWrBuffer[0] = 0;
    me->mWr = me->mHeaderSize;
}

static bool ReceiveReport(PacketCommunicationReport_t* me)
{
    me->mRd = me->mHeaderSize;
    if(!me->ReportReceive(me->mRdBuffer)) return false;
    me->mReadRemain = me->mRdBuffer[0] & REPORT_SIZE_MASK;
    if(me->mHeaderSize == 2)
    {
        me->mReadRemain <<=8;
        me->mReadRemain += me->mRdBuffer[1];
    }
    if(me->mReadRemain > (me->mBufferSize - me->mHeaderSize))
    {
        me->mReadRemain = 0;
        return false;
    }
    return true;  
}

static bool IsFirstReport(uint8_t* report)
{
    return (report[0] & IS_FIRST_REPORT_MASK) ==  IS_FIRST_REPORT_MASK;
}

static bool IsLastReport(uint8_t* report)
{
    return (report[0] & IS_LAST_REPORT_MASK) ==  IS_LAST_REPORT_MASK;
}

static void ProcessReceive(void* self)
{
    PacketCommunicationReport_t* me = (PacketCommunicationReport_t*)self;
    if(ReceiveReport(me) && (IsFirstReport(me->mRdBuffer)))
    {
        PacketCommunication_Receive(&me->packetCommunication);
    }
}

static bool IsPacketEnded(void* self)
{
    PacketCommunicationReport_t* me = (PacketCommunicationReport_t*)self;
    return IsLastReport(me->mRdBuffer) && (me->mReadRemain == 0);
}

static void WriteEndToken(void* self)
{
    PacketCommunicationReport_t* me = (PacketCommunicationReport_t*)self;
    me->mWrBuffer[0] |= IS_LAST_REPORT_MASK;
    SendReport(me);
}

static void WriteByte(void* self, uint8_t data)
{
    PacketCommunicationReport_t* me = (PacketCommunicationReport_t*)self;
    if(me->mWr == me->mBufferSize)
    {        
        SendReport(me);
    }
    me->mWrBuffer[me->mWr++] = data;

}

static bool ReadByte(void* self, uint8_t* v)
{
    PacketCommunicationReport_t* me = (PacketCommunicationReport_t*)self;
    if(me->mRd == me->mBufferSize)
    {
        if(IsLastReport(me->mRdBuffer) ||
           !ReceiveReport(me)          ||
           IsFirstReport(me->mRdBuffer) ) return false;
    }
    *v = me->mRdBuffer[me->mRd++];
    if(me->mReadRemain == 0)
    {
        return false;
    }
    me->mReadRemain--;
    return true;
}

static void WriteStartToken(void* self)
{
    PacketCommunicationReport_t* me = (PacketCommunicationReport_t*)self;
    me->mWrBuffer[0] = IS_FIRST_REPORT_MASK;
    me->mWr = me->mHeaderSize;
}

static void HandleReceiveError(void* self)
{
}

void CreatePacketCommunicationReportIntern(PacketCommunicationReport_t* definition, void (*Send)(uint8_t* v, uint16_t l), bool (*Receive)(uint8_t* v), uint8_t* rdBuffer,uint8_t* wrBuffer, uint16_t bufferSize)
{
    memset(definition, sizeof(*definition),0);

    CreatePacketCommunication(&definition->packetCommunication);

    definition->ReportSend = Send;
    definition->ReportReceive = Receive;
    definition->mBufferSize = bufferSize;
    definition->mHeaderSize = REPORT_HEADER_SIZE(bufferSize);
    definition->mRdBuffer = rdBuffer;
    definition->mWrBuffer = wrBuffer;
    definition->packetCommunication.self = definition;

    definition->packetCommunication.ProcessReceive = ProcessReceive;
	definition->packetCommunication.IsPacketEnded = IsPacketEnded;
    definition->packetCommunication.WriteStartToken = WriteStartToken;
    definition->packetCommunication.WriteEndToken = WriteEndToken;
    definition->packetCommunication.HandleReceiveError = HandleReceiveError;
    definition->packetCommunication.WriteByte = WriteByte;
    definition->packetCommunication.ReadByte = ReadByte;
}