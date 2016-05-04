/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef BAR_H
#define BAR_H

#include "PacketCommunication.h"
#include "KeyId.h"
#include "Array.h"
#include "PCString.h"

/* These functions are only valid for the proxy */
void Bar_Init(PacketCommunicationDefinition_t* pc);
void Bar_DeInit(PacketCommunicationDefinition_t* pc);


KeyId Bar_GetKeyId(void);
void Bar_SetKeyId(KeyId* kid);
void Bar_SetGetKeyId(KeyId* kid);

#endif /*BAR_H*/
