#include "Report.h"
#include "PacketCommunicationReport.h"
#include "../PacketCommunicationDefines.h"
#include <string.h>

static uint8_t buffer[1024*16];
static uint16_t wrIndex = 0;
static uint16_t rdIndex = 0;
static PacketCommunicationReport_t* pcomm=0;

void ReportInit(PacketCommunicationDefinition_t* pc)
{
    pcomm = (PacketCommunicationReport_t*)pc->self;
}

void ReportWrite(uint8_t* data, uint16_t len)
{
    memcpy(&buffer[wrIndex], data, pcomm->mBufferSize);
    wrIndex+=pcomm->mBufferSize;
    
    // trigger receive. Only needed for testing.
    if(data[0] & IS_LAST_REPORT_MASK)
    {
        pcomm->packetCommunication.ProcessReceive(pcomm);
    }
}

bool ReportRead(uint8_t* v)
{
    if(rdIndex>=wrIndex)
    {
        return false;
    }
    memcpy(v, &buffer[rdIndex], pcomm->mBufferSize);
    rdIndex+=pcomm->mBufferSize;
    if(rdIndex == wrIndex)
    {
        rdIndex = wrIndex = 0;
        buffer[rdIndex] = 0;
    }
    return true;
}
