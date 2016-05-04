/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef PWM_H
#define PWM_H

#include "PacketCommunication.h"
#include "Array.h"
#include "PCString.h"

/* These functions are only valid for the proxy */
void PWM_Init(PacketCommunicationDefinition_t* pc);
void PWM_DeInit(PacketCommunicationDefinition_t* pc);


void PWM_SetPwm(uint8_t channel, uint16_t value);
uint16_t PWM_GetPwm(uint8_t channel);
void PWM_ResetPwm(void);

#endif /*PWM_H*/
