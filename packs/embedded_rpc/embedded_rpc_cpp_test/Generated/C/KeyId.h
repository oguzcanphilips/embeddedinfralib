/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef KEYID_H
#define KEYID_H

#include "PacketCommunication.h"
#include "Array.h"
#include "PCString.h"
#include "Scope.h"

typedef struct
{
  uint32_t id;
  uint16_t key[10];
  Scope scope;
} KeyId;

void PacketCommunication_Write_KeyId(PacketCommunicationDefinition_t* pc, KeyId* value);
bool PacketCommunication_Read_KeyId(PacketCommunicationDefinition_t* pc, KeyId* value);

#endif /*KEYID_H*/
