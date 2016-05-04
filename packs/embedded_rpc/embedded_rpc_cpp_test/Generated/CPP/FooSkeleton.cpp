/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "FooSkeleton.hpp"

namespace erpc
{
FooSkeleton::FooSkeleton()
: Callback(132) // Interface Id
, mImpl(0)
{
}

FooSkeleton::~FooSkeleton()
{
}

void FooSkeleton::SetImpl(IFoo& impl)
{
  mImpl = &impl;
}

void FooSkeleton::Receive(PacketCommunication& pc)
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
    case 1: // DoThat
    {
      uint32_t i;
      uint8_t messageCnt;
      if(!pc.Read(messageCnt)) return;
      if(!pc.Read(i)) return;
      if(!pc.ReadDone()) return;
      uint32_t ret = uint32_t();
      if(mImpl) ret = mImpl->DoThat(i);
      pc.PacketStart(4, 1);
      pc.Write(messageCnt);
      pc.Write(ret);
      pc.PacketDone();
      break;
    }
    case 2: // UpperCase
    {
      PCString str;
      uint8_t messageCnt;
      if(!pc.Read(messageCnt)) return;
      if(!pc.Read(str)) return;
      if(!pc.ReadDone()) return;
      PCString ret = PCString();
      if(mImpl) ret = mImpl->UpperCase(str);
      pc.PacketStart(4, 2);
      pc.Write(messageCnt);
      pc.Write(ret);
      pc.PacketDone();
      break;
    }
    default:
      break;
  }
}
}
