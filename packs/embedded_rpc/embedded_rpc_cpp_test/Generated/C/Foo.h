/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef FOO_H
#define FOO_H

#include "PacketCommunication.h"
#include "Array.h"
#include "PCString.h"

/* These functions are only valid for the proxy */
void Foo_Init(PacketCommunicationDefinition_t* pc);
void Foo_DeInit(PacketCommunicationDefinition_t* pc);


void Foo_DoThis(Array* array);
uint32_t Foo_DoThat(uint32_t i);
PCString Foo_UpperCase(PCString* str);

#endif /*FOO_H*/
