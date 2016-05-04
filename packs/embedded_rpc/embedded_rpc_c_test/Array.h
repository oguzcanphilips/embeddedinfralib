#ifndef ARRAY_H
#define ARRAY_H

#include "PacketCommunication.h"

typedef struct
{
    uint8_t data[128];
}array_t;

void PacketCommunication_Write_Array(PacketCommunicationDefinition_t* pc, array_t* array);
bool PacketCommunication_Read_Array(PacketCommunicationDefinition_t* pc, array_t* array);

#endif