#include "PCString.h"
#include <string.h>


bool PacketCommunication_Read_PCString(PacketCommunicationDefinition_t* pc, PCString* str)
{
    uint8_t i = 0;
    uint8_t c;

    while(PacketCommunication_Read_uint8_t(pc, &c))
    {
        str->text[i++] = (char)c;
        if(c==0) return true;
        if(i==PCSTRING_LEN) return false;
    }
    return false;
}

void PacketCommunication_Write_PCString(PacketCommunicationDefinition_t* pc, PCString* str)
{
    str->text[PCSTRING_LEN-1] = 0;
    PacketCommunication_Write(pc, (uint8_t*)str->text, strlen(str->text)+1);
}