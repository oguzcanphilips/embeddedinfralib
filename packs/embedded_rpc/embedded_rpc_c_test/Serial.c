#include "Serial.h"
#include "../PacketCommunicationDefines.h"

static uint8_t buffer1[1024];
static uint16_t wrIndex1 = 0;
static uint16_t rdIndex1 = 0;

static uint8_t buffer2[1024];
static uint16_t wrIndex2 = 0;
static uint16_t rdIndex2 = 0;
static PacketCommunicationDefinition_t* pcomm1 = 0;
static PacketCommunicationDefinition_t* pcomm2 = 0;

void Serial1Init(PacketCommunicationDefinition_t* pc)
{
    pcomm1 = pc;
}

void Serial1Write(uint8_t data)
{
    buffer1[wrIndex1++] =  data;
    
    // trigger receive. Only needed for testing.
    if(data==SLIP_END)//WriteEndToken
    {
        pcomm1->ProcessReceive(pcomm1->self);
    }
}

bool Serial1Read(uint8_t* v)
{
    if(rdIndex1>=wrIndex1)
    {
        return false;
    }
    *v = buffer1[rdIndex1++];
    if(rdIndex1 == wrIndex1)
    {
        rdIndex1 = wrIndex1 = 0;
        buffer1[rdIndex1] = 0;
    }
    return true;
}

void Serial2Init(PacketCommunicationDefinition_t* pc)
{
    pcomm2 = pc;
}

void Serial2Write(uint8_t data)
{
    buffer2[wrIndex2++] = data;

    // trigger receive. Only needed for testing.
    if (data == SLIP_END)//WriteEndToken
    {
        pcomm2->ProcessReceive(pcomm2->self);
    }
}

bool Serial2Read(uint8_t* v)
{
    if (rdIndex2 >= wrIndex2)
    {
        return false;
    }
    *v = buffer2[rdIndex2++];
    if (rdIndex2 == wrIndex2)
    {
        rdIndex2 = wrIndex2 = 0;
        buffer2[rdIndex2] = 0;
    }
    return true;
}
