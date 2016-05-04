/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWMEventsSkeleton.hpp"

PWMEventsSkeleton::PWMEventsSkeleton(PacketCommunication^ packetComm)
: Callback(129) // Interface Id
, mPacketComm(packetComm)
{
  mPacketComm->Register(this);
}

PWMEventsSkeleton::~PWMEventsSkeleton()
{
  mPacketComm->Unregister(this);
}

void PWMEventsSkeleton::SetImpl(IPWMEvents^ impl)
{
  mImpl = impl;
}

void PWMEventsSkeleton::Receive(PacketCommunication^ pc)
{
  uint8_t func;
  if(!pc->Read(func)) return;
  switch(func)
  {
    case 0:
    {
      uint8_t channel = uint8_t();
      uint16_t value = uint16_t();
      if(!pc->Read(channel)) return;
      if(!pc->Read(value)) return;
      if(!pc->ReadDone()) return;
      if(mImpl) mImpl->PwmUpdate(channel, value);
      break;
    }
    default:
      break;
  }
}
