/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "KeyId.h"

void PacketCommunication_Write_KeyId(PacketCommunicationDefinition_t* pc, KeyId* value)
{
  PacketCommunication_Write_uint32_t(pc, value->id);
  {
    uint32_t i;
    for(i=0;i<10;++i) PacketCommunication_Write_uint16_t(pc, value->key[i]);
  }
  PacketCommunication_Write_Scope(pc, &value->scope);
}

bool PacketCommunication_Read_KeyId(PacketCommunicationDefinition_t* pc, KeyId* value)
{
  if(!PacketCommunication_Read_uint32_t(pc, &value->id)) return false;
  {
    uint32_t i;
    for(i=0;i<10;++i) if(!PacketCommunication_Read_uint16_t(pc, &value->key[i])) return false;
  }
  if(!PacketCommunication_Read_Scope(pc, &value->scope)) return false;
  return true;
}
