#ifndef PACKETCOMMUNICATIONREPORT_H
#define PACKETCOMMUNICATIONREPORT_H

/* RFC 1055 */
#include "PacketCommunication.h"

typedef struct PacketCommunicationReport
{
    PacketCommunicationDefinition_t packetCommunication;
    void (*ReportSend)(uint8_t* v, uint16_t len);
    bool (*ReportReceive)(uint8_t* v);
    uint16_t mWr;
    uint16_t mRd;
    uint16_t mReadRemain;
    uint16_t mBufferSize;
    uint16_t mHeaderSize;
    uint8_t* mWrBuffer;
    uint8_t* mRdBuffer;
} PacketCommunicationReport_t;

void CreatePacketCommunicationReportIntern(PacketCommunicationReport_t* definition, void (*Send)(uint8_t* v, uint16_t l), bool (*Receive)(uint8_t* v), uint8_t* rdBuffer,uint8_t* wrBuffer, uint16_t bufferSize);

#define CreatePacketCommunicationReport(definitionPointer, sendFunc, receiveFunc, ReportSize)\
{\
    static uint8_t rdBuffer[ReportSize];\
    static uint8_t wrBuffer[ReportSize];\
    CreatePacketCommunicationReportIntern(definitionPointer, sendFunc, receiveFunc, rdBuffer, wrBuffer, ReportSize);\
}
#endif