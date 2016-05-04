/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWMEventsSkeleton.h"
#include "PWMEvents.h"

static void Receive(PacketCommunicationDefinition_t* pc);
static packetCommunicationCallback_t callback = {129, 0, 0, &Receive};


void PWMEventsSkeleton_Init(PacketCommunicationDefinition_t* pc)
{
  PacketCommunication_Register(pc, &callback);
}

void PWMEventsSkeleton_DeInit(PacketCommunicationDefinition_t* pc)
{
  PacketCommunication_Unregister(pc, &callback);
}

static void Receive(PacketCommunicationDefinition_t* pc)
{
  uint8_t func;
  if(!PacketCommunication_Read_uint8_t(pc, &func)) return;
  switch(func)
  {
    case 0:
    {
      uint8_t channel;
      uint16_t value;
      if(!PacketCommunication_Read_uint8_t(pc, &channel)) return;
      if(!PacketCommunication_Read_uint16_t(pc, &value)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        PWMEvents_PwmUpdate(channel, value);
      };
      break;
    }
    default:
      break;
  }
}
