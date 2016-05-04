#include "Serial.h"
#include "../PacketCommunicationDefines.h"

static uint8_t buffer[1024];
static uint16_t wrIndex = 0;
static uint16_t rdIndex = 0;
static PacketCommunicationDefinition_t* pcomm=0;
void SerialInit(PacketCommunicationDefinition_t* pc)
{
    pcomm = pc;
}

void SerialWrite(uint8_t data)
{
    buffer[wrIndex++] =  data;
    
    // trigger receive. Only needed for testing.
    if(data==SLIP_END)//PacketEnd
    {
        pcomm->ProcessReceive(pcomm->self);
    }
}

bool SerialRead(uint8_t* v)
{
    if(rdIndex>=wrIndex)
    {
        return false;
    }
    *v = buffer[rdIndex++];
    if(rdIndex == wrIndex)
    {
        rdIndex = wrIndex = 0;
        buffer[rdIndex] = 0;
    }
    return true;
}
