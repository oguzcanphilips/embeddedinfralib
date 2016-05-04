/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "BarSkeleton.hpp"

BarSkeleton::BarSkeleton(PacketCommunication^ packetComm)
: Callback(134) // Interface Id
, mPacketComm(packetComm)
{
  mPacketComm->Register(this);
}

BarSkeleton::~BarSkeleton()
{
  mPacketComm->Unregister(this);
}

void BarSkeleton::SetImpl(IBar^ impl)
{
  mImpl = impl;
}

void BarSkeleton::Receive(PacketCommunication^ pc)
{
  uint8_t func;
  if(!pc->Read(func)) return;
  switch(func)
  {
    case 0:
    {
      uint8_t messageCnt;
      if(!pc->Read(messageCnt)) return;
      if(!pc->ReadDone()) return;
      KeyId^ ret = gcnew KeyId();
      if(mImpl) ret = mImpl->GetKeyId();
      pc->PacketStart(6, 0);
      pc->Write(messageCnt);
      pc->Write(ret);
      pc->PacketDone();
      break;
    }
    case 1:
    {
      KeyId^ kid = gcnew KeyId();
      if(!pc->Read(kid)) return;
      if(!pc->ReadDone()) return;
      if(mImpl) mImpl->SetKeyId(kid);
      break;
    }
    case 2:
    {
      KeyId^ kid = gcnew KeyId();
      uint8_t messageCnt;
      if(!pc->Read(messageCnt)) return;
      if(!pc->Read(kid)) return;
      if(!pc->ReadDone()) return;
      if(mImpl) mImpl->SetGetKeyId(kid);
      pc->PacketStart(6, 2);
      pc->Write(messageCnt);
      pc->Write(kid);
      pc->PacketDone();
      break;
    }
    default:
      break;
  }
}
