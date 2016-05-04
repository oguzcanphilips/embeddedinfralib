/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWMSkeleton.h"
#include "PWM.h"

static void Receive(PacketCommunicationDefinition_t* pc);
static packetCommunicationCallback_t callback = {128, 0, 0, &Receive};


void PWMSkeleton_Init(PacketCommunicationDefinition_t* pc)
{
  PacketCommunication_Register(pc, &callback);
}

void PWMSkeleton_DeInit(PacketCommunicationDefinition_t* pc)
{
  PacketCommunication_Unregister(pc, &callback);
}

static void Receive(PacketCommunicationDefinition_t* pc)
{
  uint8_t func;
  if(!PacketCommunication_Read_uint8_t(pc, &func)) return;
  switch(func)
  {
    case 100:
    {
      uint8_t channel;
      uint16_t value;
      if(!PacketCommunication_Read_uint8_t(pc, &channel)) return;
      if(!PacketCommunication_Read_uint16_t(pc, &value)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        PWM_SetPwm(channel, value);
      };
      break;
    }
    case 101:
    {
      uint8_t channel;
      uint8_t messageCnt;
      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
      if(!PacketCommunication_Read_uint8_t(pc, &channel)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        uint16_t ret = PWM_GetPwm(channel);
        PacketCommunication_PacketStart_InterfaceFunction(pc, 0, 101);
        PacketCommunication_Write_uint8_t(pc, messageCnt);
        PacketCommunication_Write_uint16_t(pc, ret);
        PacketCommunication_PacketDone_InterfaceFunction(pc);
      };
      break;
    }
    case 102:
    {
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        PWM_ResetPwm();
      };
      break;
    }
    default:
      break;
  }
}
