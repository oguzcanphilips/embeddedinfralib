/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "TestSkeleton.h"
#include "Test.h"

static void Receive(PacketCommunicationDefinition_t* pc);
static packetCommunicationCallback_t callback = {130, 0, 0, &Receive};


void TestSkeleton_Init(PacketCommunicationDefinition_t* pc)
{
  PacketCommunication_Register(pc, &callback);
}

void TestSkeleton_DeInit(PacketCommunicationDefinition_t* pc)
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
        Test_DoThis(&array);
      };
      break;
    }
    case 1:
    {
      uint16_t i;
      uint8_t messageCnt;
      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
      if(!PacketCommunication_Read_uint16_t(pc, &i)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        uint16_t ret = Test_DoubleOutput(i);
        PacketCommunication_PacketStart_InterfaceFunction(pc, 2, 1);
        PacketCommunication_Write_uint8_t(pc, messageCnt);
        PacketCommunication_Write_uint16_t(pc, ret);
        PacketCommunication_PacketDone_InterfaceFunction(pc);
      };
      break;
    }
    case 2:
    {
      uint16_t value;
      uint8_t messageCnt;
      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
      if(!PacketCommunication_Read_uint16_t(pc, &value)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        bool ret = Test_IO_In(value);
        PacketCommunication_PacketStart_InterfaceFunction(pc, 2, 2);
        PacketCommunication_Write_uint8_t(pc, messageCnt);
        PacketCommunication_Write_bool(pc, ret);
        PacketCommunication_PacketDone_InterfaceFunction(pc);
      };
      break;
    }
    case 3:
    {
      uint16_t value;
      uint8_t messageCnt;
      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        bool ret = Test_IO_Out(&value);
        PacketCommunication_PacketStart_InterfaceFunction(pc, 2, 3);
        PacketCommunication_Write_uint8_t(pc, messageCnt);
        PacketCommunication_Write_bool(pc, ret);
        PacketCommunication_Write_uint16_t(pc, value);        PacketCommunication_PacketDone_InterfaceFunction(pc);
      };
      break;
    }
    case 4:
    {
      uint16_t value;
      uint8_t messageCnt;
      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;
      if(!PacketCommunication_Read_uint16_t(pc, &value)) return;
      if(!PacketCommunication_ReadDone(pc)) return;
      {
        Test_IO_InOut(&value);
        PacketCommunication_PacketStart_InterfaceFunction(pc, 2, 4);
        PacketCommunication_Write_uint8_t(pc, messageCnt);
        PacketCommunication_Write_uint16_t(pc, value);        PacketCommunication_PacketDone_InterfaceFunction(pc);
      };
      break;
    }
    default:
      break;
  }
}
