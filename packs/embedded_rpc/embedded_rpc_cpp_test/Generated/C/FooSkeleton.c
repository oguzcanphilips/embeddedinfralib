/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "FooSkeleton.h"
#include "Foo.h"

static void Receive(PacketCommunicationDefinition_t* pc);
static packetCommunicationCallback_t callback = {132, 0, 0, &Receive};


void FooSkeleton_Init(PacketCommunicationDefinition_t* pc)
{
  PacketCommunication_Register(pc, &callback);
}

void FooSkeleton_DeInit(PacketCommunicationDefinition_t* pc)
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
      Array array;
      if(!PacketCommunication_Read_Array(pc, &array)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        Foo_DoThis(&array);
      };
      break;
    }
    case 1:
    {
      uint32_t i;
      uint8_t messageCnt;
      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
      if(!PacketCommunication_Read_uint32_t(pc, &i)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        uint32_t ret = Foo_DoThat(i);
        PacketCommunication_PacketStart_InterfaceFunction(pc, 4, 1);
        PacketCommunication_Write_uint8_t(pc, messageCnt);
        PacketCommunication_Write_uint32_t(pc, ret);
        PacketCommunication_PacketDone_InterfaceFunction(pc);
      };
      break;
    }
    case 2:
    {
      PCString str;
      uint8_t messageCnt;
      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
      if(!PacketCommunication_Read_PCString(pc, &str)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        PCString ret = Foo_UpperCase(&str);
        PacketCommunication_PacketStart_InterfaceFunction(pc, 4, 2);
        PacketCommunication_Write_uint8_t(pc, messageCnt);
        PacketCommunication_Write_PCString(pc, &ret);
        PacketCommunication_PacketDone_InterfaceFunction(pc);
      };
      break;
    }
    default:
      break;
  }
}
