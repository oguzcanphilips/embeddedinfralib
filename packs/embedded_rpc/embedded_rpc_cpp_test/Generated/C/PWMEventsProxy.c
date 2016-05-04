/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWMEvents.h"
#include <string.h>

static PacketCommunicationDefinition_t* pcomm = 0;
void PWMEvents_Init(PacketCommunicationDefinition_t* pc)
{
  if(pcomm) return;
  pcomm = pc;
}

void PWMEvents_DeInit(PacketCommunicationDefinition_t* pc)
{
  if(pcomm != pc) return;
  pcomm = 0;
}

void PWMEvents_PwmUpdate(uint8_t channel, uint16_t value)
{
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 129, 0);
  PacketCommunication_Write_uint8_t(pcomm, channel);
  PacketCommunication_Write_uint16_t(pcomm, value);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
}
