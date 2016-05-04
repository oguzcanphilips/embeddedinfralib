/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "Foo.h"
#include <string.h>

static PacketCommunicationDefinition_t* pcomm = 0;
static volatile uint8_t receiving = false;
static volatile uint8_t pendingMessageCnt = 0;
static volatile uint8_t pendingFunctionId = 0;
static uint8_t messageCntGen = 0;
static void Receive(PacketCommunicationDefinition_t* pc);
static packetCommunicationCallback_t callback = {4, 0, 0, &Receive};

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

void Foo_Init(PacketCommunicationDefinition_t* pc)
{
  if(pcomm) return;
  pcomm = pc;
  PacketCommunication_Register(pcomm, &callback);
}

void Foo_DeInit(PacketCommunicationDefinition_t* pc)
{
  if(pcomm != pc) return;
  PacketCommunication_Unregister(pcomm, &callback);
  pcomm = 0;
}

void Foo_DoThis(Array* array)
{
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 132, 0);
  PacketCommunication_Write_Array(pcomm, array);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
}

uint32_t Foo_DoThat(uint32_t i)
{
  receiving = true;
  pendingFunctionId = 1;
  pendingMessageCnt = messageCntGen++;
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 132, 1);
  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);
  PacketCommunication_Write_uint32_t(pcomm, i);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
  if(receiving) pcomm->ProcessReceive(pcomm);

  {
    uint32_t ret;
    if(receiving
      || !PacketCommunication_Read_uint32_t(pcomm, &ret)
      || !PacketCommunication_ReadDone(pcomm))
    {
      memset((void*)&ret, 0, sizeof(ret));
      pcomm->HandleReceiveError(pcomm);
    }
    return ret;
  }
}

PCString Foo_UpperCase(PCString* str)
{
  receiving = true;
  pendingFunctionId = 2;
  pendingMessageCnt = messageCntGen++;
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 132, 2);
  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);
  PacketCommunication_Write_PCString(pcomm, str);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
  if(receiving) pcomm->ProcessReceive(pcomm);

  {
    PCString ret;
    if(receiving
      || !PacketCommunication_Read_PCString(pcomm, &ret)
      || !PacketCommunication_ReadDone(pcomm))
    {
      memset((void*)&ret, 0, sizeof(ret));
      pcomm->HandleReceiveError(pcomm);
    }
    return ret;
  }
}
