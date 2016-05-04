/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "Test.h"
#include <string.h>

static PacketCommunicationDefinition_t* pcomm = 0;
static volatile uint8_t receiving = false;
static volatile uint8_t pendingMessageCnt = 0;
static volatile uint8_t pendingFunctionId = 0;
static uint8_t messageCntGen = 0;
static void Receive(PacketCommunicationDefinition_t* pc);
static packetCommunicationCallback_t callback = {2, 0, 0, &Receive};

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

void Test_Init(PacketCommunicationDefinition_t* pc)
{
  if(pcomm) return;
  pcomm = pc;
  PacketCommunication_Register(pcomm, &callback);
}

void Test_DeInit(PacketCommunicationDefinition_t* pc)
{
  if(pcomm != pc) return;
  PacketCommunication_Unregister(pcomm, &callback);
  pcomm = 0;
}

void Test_DoThis(Array* array)
{
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 130, 0);
  PacketCommunication_Write_Array(pcomm, array);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
}

uint16_t Test_DoubleOutput(uint16_t i)
{
  receiving = true;
  pendingFunctionId = 1;
  pendingMessageCnt = messageCntGen++;
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 130, 1);
  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);
  PacketCommunication_Write_uint16_t(pcomm, i);
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

bool Test_IO_In(uint16_t value)
{
  receiving = true;
  pendingFunctionId = 2;
  pendingMessageCnt = messageCntGen++;
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 130, 2);
  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);
  PacketCommunication_Write_uint16_t(pcomm, value);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
  if(receiving) pcomm->ProcessReceive(pcomm);

  {
    bool ret;
    if(receiving
      || !PacketCommunication_Read_bool(pcomm, &ret)
      || !PacketCommunication_ReadDone(pcomm))
    {
      memset((void*)&ret, 0, sizeof(ret));
      pcomm->HandleReceiveError(pcomm);
    }
    return ret;
  }
}

bool Test_IO_Out(uint16_t* value)
{
  receiving = true;
  pendingFunctionId = 3;
  pendingMessageCnt = messageCntGen++;
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 130, 3);
  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
  if(receiving) pcomm->ProcessReceive(pcomm);

  {
    bool ret;
    if(receiving
      || !PacketCommunication_Read_bool(pcomm, &ret)
      || !PacketCommunication_Read_uint16_t(pcomm, value)
      || !PacketCommunication_ReadDone(pcomm))
    {
      memset((void*)&ret, 0, sizeof(ret));
      memset((void*)value,0,sizeof(*value));
      pcomm->HandleReceiveError(pcomm);
    }
    return ret;
  }
}

void Test_IO_InOut(uint16_t* value)
{
  receiving = true;
  pendingFunctionId = 4;
  pendingMessageCnt = messageCntGen++;
  PacketCommunication_PacketStart_InterfaceFunction(pcomm, 130, 4);
  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);
  PacketCommunication_Write_uint16_t(pcomm, *value);
  PacketCommunication_PacketDone_InterfaceFunction(pcomm);
  if(receiving) pcomm->ProcessReceive(pcomm);

  {
    if(receiving
      || !PacketCommunication_Read_uint16_t(pcomm, value)
      || !PacketCommunication_ReadDone(pcomm))
    {
      memset((void*)value,0,sizeof(*value));
      pcomm->HandleReceiveError(pcomm);
    }
  }
}
