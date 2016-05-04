/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "Bar.h"
#include <string.h>

static PacketCommunicationDefinition_t* pcomm = 0;
static volatile uint8_t receiving = false;
static volatile uint8_t pendingMessageCnt = 0;
static volatile uint8_t pendingFunctionId = 0;
static uint8_t messageCntGen = 0;
static void Receive(PacketCommunicationDefinition_t* pc);
static packetCommunicationCallback_t callback = {6, 0, 0, &Receive};

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

void Bar_Init(PacketCommunicationDefinition_t* pc)
{
  if(pcomm) return;
  pcomm = pc;
  PacketCommunication_Register(pcomm, &callback);
}

void Bar_DeInit(PacketCommunicationDefinition_t* pc)
{
  if(pcomm != pc) return;
  PacketCommunication_Unregister(pcomm, &callback);
  pcomm = 0;
}

KeyId Bar_GetKeyId(void)
{
  receiving = true;
  pendingFunctionId = 0;
  pendingMessageCnt = messageCntGen++;
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 134, 0);
  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
  if(receiving) pcomm->ProcessReceive(pcomm);

  {
    KeyId ret;
    if(receiving
      || !PacketCommunication_Read_KeyId(pcomm, &ret)
      || !PacketCommunication_ReadDone(pcomm))
    {
      memset((void*)&ret, 0, sizeof(ret));
      pcomm->HandleReceiveError(pcomm);
    }
    return ret;
  }
}

void Bar_SetKeyId(KeyId* kid)
{
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 134, 1);
  PacketCommunication_Write_KeyId(pcomm, kid);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
}

void Bar_SetGetKeyId(KeyId* kid)
{
  receiving = true;
  pendingFunctionId = 2;
  pendingMessageCnt = messageCntGen++;
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 134, 2);
  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);
  PacketCommunication_Write_KeyId(pcomm, *kid);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
  if(receiving) pcomm->ProcessReceive(pcomm);

  {
    if(receiving
      || !PacketCommunication_Read_KeyId(pcomm, kid)
      || !PacketCommunication_ReadDone(pcomm))
    {
      memset((void*)kid,0,sizeof(*kid));
      pcomm->HandleReceiveError(pcomm);
    }
  }
}
