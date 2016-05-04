/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWM.h"
#include <string.h>

static PacketCommunicationDefinition_t* pcomm = 0;
static volatile uint8_t receiving = false;
static volatile uint8_t pendingMessageCnt = 0;
static volatile uint8_t pendingFunctionId = 0;
static uint8_t messageCntGen = 0;
static void Receive(PacketCommunicationDefinition_t* pc);
static packetCommunicationCallback_t callback = {0, 0, 0, &Receive};

static void Receive(PacketCommunicationDefinition_t* pc)
{
  uint8_t messageCnt;
  uint8_t functionId;
  if(!PacketCommunication_Read_uint8_t(pc, &functionId)) return;
  if(pendingFunctionId != functionId) return;
  if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
  if(pendingMessageCnt != messageCnt) return;
  receiving = false;
}

void PWM_Init(PacketCommunicationDefinition_t* pc)
{
  if(pcomm) return;
  pcomm = pc;
  PacketCommunication_Register(pcomm, &callback);
}

void PWM_DeInit(PacketCommunicationDefinition_t* pc)
{
  if(pcomm != pc) return;
  PacketCommunication_Unregister(pcomm, &callback);
  pcomm = 0;
}

void PWM_SetPwm(uint8_t channel, uint16_t value)
{
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 128, 100);
  PacketCommunication_Write_uint8_t(pcomm, channel);
  PacketCommunication_Write_uint16_t(pcomm, value);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
}

uint16_t PWM_GetPwm(uint8_t channel)
{
  receiving = true;
  pendingFunctionId = 101;
  pendingMessageCnt = messageCntGen++;
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 128, 101);
  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);
  PacketCommunication_Write_uint8_t(pcomm, channel);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
  if(receiving) pcomm->ProcessReceive(pcomm);

  {
    uint16_t ret;
    if(receiving
      || !PacketCommunication_Read_uint16_t(pcomm, &ret)
      || !PacketCommunication_ReadDone(pcomm))
    {
      memset((void*)&ret, 0, sizeof(ret));
      pcomm->HandleReceiveError(pcomm);
    }
    return ret;
  }
}

void PWM_ResetPwm(void)
{
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 128, 102);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
}
