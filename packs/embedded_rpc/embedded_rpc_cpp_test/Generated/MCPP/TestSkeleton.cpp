/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "TestSkeleton.hpp"

TestSkeleton::TestSkeleton(PacketCommunication^ packetComm)
: Callback(130) // Interface Id
, mPacketComm(packetComm)
{
  mPacketComm->Register(this);
}

TestSkeleton::~TestSkeleton()
{
  mPacketComm->Unregister(this);
}

void TestSkeleton::SetImpl(ITest^ impl)
{
  mImpl = impl;
}

void TestSkeleton::Receive(PacketCommunication^ pc)
{
  uint8_t func;
  if(!pc->Read(func)) return;
  switch(func)
  {
    case 0:
    {
      Array^ array = gcnew Array();
      if(!pc->Read(array)) return;
      if(!pc->ReadDone()) return;
      if(mImpl) mImpl->DoThis(array);
      break;
    }
    case 1:
    {
      uint16_t i = uint16_t();
      uint8_t messageCnt;
      if(!pc->Read(messageCnt)) return;
      if(!pc->Read(i)) return;
      if(!pc->ReadDone()) return;
      uint16_t ret = uint16_t();
      if(mImpl) ret = mImpl->DoubleOutput(i);
      pc->PacketStart(2, 1);
      pc->Write(messageCnt);
      pc->Write(ret);
      pc->PacketDone();
      break;
    }
    case 2:
    {
      uint16_t value = uint16_t();
      uint8_t messageCnt;
      if(!pc->Read(messageCnt)) return;
      if(!pc->Read(value)) return;
      if(!pc->ReadDone()) return;
      bool ret = bool();
      if(mImpl) ret = mImpl->IO_In(value);
      pc->PacketStart(2, 2);
      pc->Write(messageCnt);
      pc->Write(ret);
      pc->PacketDone();
      break;
    }
    case 3:
    {
      uint16_t value = uint16_t();
      uint8_t messageCnt;
      if(!pc->Read(messageCnt)) return;
      if(!pc->ReadDone()) return;
      bool ret = bool();
      if(mImpl) ret = mImpl->IO_Out(value);
      pc->PacketStart(2, 3);
      pc->Write(messageCnt);
      pc->Write(ret);
      pc->Write(value);
      pc->PacketDone();
      break;
    }
    case 4:
    {
      uint16_t value = uint16_t();
      uint8_t messageCnt;
      if(!pc->Read(messageCnt)) return;
      if(!pc->Read(value)) return;
      if(!pc->ReadDone()) return;
      if(mImpl) mImpl->IO_InOut(value);
      pc->PacketStart(2, 4);
      pc->Write(messageCnt);
      pc->Write(value);
      pc->PacketDone();
      break;
    }
    default:
      break;
  }
}
