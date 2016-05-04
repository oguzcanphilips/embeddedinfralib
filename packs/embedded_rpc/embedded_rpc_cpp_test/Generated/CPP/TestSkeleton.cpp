/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "TestSkeleton.hpp"

namespace erpc
{
TestSkeleton::TestSkeleton()
: Callback(130) // Interface Id
, mImpl(0)
{
}

TestSkeleton::~TestSkeleton()
{
}

void TestSkeleton::SetImpl(ITest& impl)
{
  mImpl = &impl;
}

void TestSkeleton::Receive(PacketCommunication& pc)
{
  uint8_t func;
  if(!pc.Read(func)) return;
  switch(func)
  {
    case 0: // DoThis
    {
      Array array;
      if(!pc.Read(array)) return;
      if(!pc.ReadDone()) return;
      if(mImpl) mImpl->DoThis(array);
      break;
    }
    case 1: // DoubleOutput
    {
      uint16_t i;
      uint8_t messageCnt;
      if(!pc.Read(messageCnt)) return;
      if(!pc.Read(i)) return;
      if(!pc.ReadDone()) return;
      uint16_t ret = uint16_t();
      if(mImpl) ret = mImpl->DoubleOutput(i);
      pc.PacketStart(2, 1);
      pc.Write(messageCnt);
      pc.Write(ret);
      pc.PacketDone();
      break;
    }
    case 2: // IO_In
    {
      uint16_t value;
      uint8_t messageCnt;
      if(!pc.Read(messageCnt)) return;
      if(!pc.Read(value)) return;
      if(!pc.ReadDone()) return;
      bool ret = bool();
      if(mImpl) ret = mImpl->IO_In(value);
      pc.PacketStart(2, 2);
      pc.Write(messageCnt);
      pc.Write(ret);
      pc.PacketDone();
      break;
    }
    case 3: // IO_Out
    {
      uint16_t value;
      uint8_t messageCnt;
      if(!pc.Read(messageCnt)) return;
      if(!pc.ReadDone()) return;
      bool ret = bool();
      if(mImpl) ret = mImpl->IO_Out(value);
      pc.PacketStart(2, 3);
      pc.Write(messageCnt);
      pc.Write(ret);
      pc.Write(value);
      pc.PacketDone();
      break;
    }
    case 4: // IO_InOut
    {
      uint16_t value;
      uint8_t messageCnt;
      if(!pc.Read(messageCnt)) return;
      if(!pc.Read(value)) return;
      if(!pc.ReadDone()) return;
      if(mImpl) mImpl->IO_InOut(value);
      pc.PacketStart(2, 4);
      pc.Write(messageCnt);
      pc.Write(value);
      pc.PacketDone();
      break;
    }
    default:
      break;
  }
}
}
