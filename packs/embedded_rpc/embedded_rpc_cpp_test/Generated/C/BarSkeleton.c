/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "BarSkeleton.h"
#include "Bar.h"

static void Receive(PacketCommunicationDefinition_t* pc);
static packetCommunicationCallback_t callback = {134, 0, 0, &Receive};


void BarSkeleton_Init(PacketCommunicationDefinition_t* pc)
{
  PacketCommunication_Register(pc, &callback);
}

void BarSkeleton_DeInit(PacketCommunicationDefinition_t* pc)
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
      uint8_t messageCnt;
      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        KeyId ret = Bar_GetKeyId();
        PacketCommunication_PacketStart_InterfaceFunction(pc, 6, 0);
        PacketCommunication_Write_uint8_t(pc, messageCnt);
        PacketCommunication_Write_KeyId(pc, &ret);
        PacketCommunication_PacketDone_InterfaceFunction(pc);
      };
      break;
    }
    case 1:
    {
      KeyId kid;
      if(!PacketCommunication_Read_KeyId(pc, &kid)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        Bar_SetKeyId(&kid);
      };
      break;
    }
    case 2:
    {
      KeyId kid;
      uint8_t messageCnt;
      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
      if(!PacketCommunication_Read_KeyId(pc, &kid)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        Bar_SetGetKeyId(&kid);
        PacketCommunication_PacketStart_InterfaceFunction(pc, 6, 2);
        PacketCommunication_Write_uint8_t(pc, messageCnt);
        PacketCommunication_Write_KeyId(pc, kid);        PacketCommunication_PacketDone_InterfaceFunction(pc);
      };
      break;
    }
    default:
      break;
  }
}
