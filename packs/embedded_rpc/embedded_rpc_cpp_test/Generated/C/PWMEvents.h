/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef PWMEVENTS_H
#define PWMEVENTS_H

#include "PacketCommunication.h"
#include "Array.h"
#include "PCString.h"

/* These functions are only valid for the proxy */
void PWMEvents_Init(PacketCommunicationDefinition_t* pc);
void PWMEvents_DeInit(PacketCommunicationDefinition_t* pc);


void PWMEvents_PwmUpdate(uint8_t channel, uint16_t value);

#endif /*PWMEVENTS_H*/
