#ifndef PCSTRING_H
#define PCSTRING_H

#include "PacketCommunication.h"

typedef struct PCString
{
    char text[PCSTRING_LEN];
} PCString;

bool PacketCommunication_Read_PCString(PacketCommunicationDefinition_t* pc, PCString* str);
void PacketCommunication_Write_PCString(PacketCommunicationDefinition_t* pc, PCString* str);

#endif