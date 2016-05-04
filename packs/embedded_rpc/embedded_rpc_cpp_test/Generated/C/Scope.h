/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef SCOPE_H
#define SCOPE_H

#include "PacketCommunication.h"

typedef enum
{
  Scope_Low = 0,
  Scope_Med = 111,
  Scope_High = 112
} Scope;

static inline void PacketCommunication_Write_Scope(PacketCommunicationDefinition_t* pc, Scope* value)
{
  PacketCommunication_Write_int32_t(pc, (int32_t)*value);
}
static inline bool PacketCommunication_Read_Scope(PacketCommunicationDefinition_t* pc, Scope* value)
{
  int32_t tmp;
  if(!PacketCommunication_Read_int32_t(pc, &tmp)) return false;
  *value = (Scope)tmp;
  return true;
}

#endif /*SCOPE_H*/
