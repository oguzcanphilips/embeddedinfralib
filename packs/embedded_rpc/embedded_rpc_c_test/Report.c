#include "Report.h"
#include "PacketCommunicationReport.h"
#include "../PacketCommunicationDefines.h"
#include <string.h>

static uint8_t buffer1[1024 * 16];
static uint16_t wrIndex1 = 0;
static uint16_t rdIndex1 = 0;
static PacketCommunicationReport_t* pcomm1 = 0;

static uint8_t buffer2[1024 * 16];
static uint16_t wrIndex2 = 0;
static uint16_t rdIndex2 = 0;
static PacketCommunicationReport_t* pcomm2 = 0;

void Report1Init(PacketCommunicationDefinition_t* pc)
{
    pcomm1 = (PacketCommunicationReport_t*)pc->self;
}

void Report1Write(uint8_t* data, uint16_t len)
{
    memcpy(&buffer1[wrIndex1], data, pcomm1->mBufferSize);
    wrIndex1+=pcomm1->mBufferSize;
    
    // trigger receive. Only needed for testing.
    if(data[0] & IS_LAST_REPORT_MASK)
    {
        pcomm1->packetCommunication.ProcessReceive(pcomm1);
    }
}

bool Report1Read(uint8_t* v)
{
    if(rdIndex1>=wrIndex1)
    {
        return false;
    }
    memcpy(v, &buffer1[rdIndex1], pcomm1->mBufferSize);
    rdIndex1+=pcomm1->mBufferSize;
    if(rdIndex1 == wrIndex1)
    {
        rdIndex1 = wrIndex1 = 0;
        buffer1[rdIndex1] = 0;
    }
    return true;
}

void Report2Init(PacketCommunicationDefinition_t* pc)
{
    pcomm2 = (PacketCommunicationReport_t*)pc->self;
}

void Report2Write(uint8_t* data, uint16_t len)
{
    memcpy(&buffer2[wrIndex2], data, pcomm2->mBufferSize);
    wrIndex2 += pcomm2->mBufferSize;

    // trigger receive. Only needed for testing.
    if (data[0] & IS_LAST_REPORT_MASK)
    {
        pcomm2->packetCommunication.ProcessReceive(pcomm2);
    }
}

bool Report2Read(uint8_t* v)
{
    if (rdIndex2 >= wrIndex2)
    {
        return false;
    }
    memcpy(v, &buffer2[rdIndex2], pcomm2->mBufferSize);
    rdIndex2 += pcomm2->mBufferSize;
    if (rdIndex2 == wrIndex2)
    {
        rdIndex2 = wrIndex2 = 0;
        buffer2[rdIndex2] = 0;
    }
    return true;
}
