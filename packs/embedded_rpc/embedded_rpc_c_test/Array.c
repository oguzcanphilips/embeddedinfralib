#include "Array.h"
#include <string.h>


void PacketCommunication_Write_Array(PacketCommunicationDefinition_t* pc, array_t* array)
{
    PacketCommunication_Write(pc, array->data, sizeof(array->data));
}

bool PacketCommunication_Read_Array(PacketCommunicationDefinition_t* pc, array_t* array)
{
    return PacketCommunication_Read(pc, array->data, sizeof(array->data));
}
