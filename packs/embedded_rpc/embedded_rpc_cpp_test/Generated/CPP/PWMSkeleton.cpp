/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWMSkeleton.hpp"

namespace erpc
{
PWMSkeleton::PWMSkeleton(PacketCommunication& packetCommEvents)
: Callback(128) // Interface Id
, PwmUpdate(&mEventsProxy, &PWMEventsProxy::PwmUpdate)
, mImpl(0)
, mEventsProxy(packetCommEvents)
{
}

PWMSkeleton::~PWMSkeleton()
{
}

void PWMSkeleton::SetImpl(IPWM& impl)
{
  mImpl = &impl;
  mImpl->PwmUpdateSignal += PwmUpdate;
}

void PWMSkeleton::Receive(PacketCommunication& pc)
{
  uint8_t func;
  if(!pc.Read(func)) return;
  switch(func)
  {
    case 100: // SetPwm
    {
      uint8_t channel;
      uint16_t value;
      if(!pc.Read(channel)) return;
      if(!pc.Read(value)) return;
      if(!pc.ReadDone()) return;
      if(mImpl) mImpl->SetPwm(channel, value);
      break;
    }
    case 101: // GetPwm
    {
      uint8_t channel;
      uint8_t messageCnt;
      if(!pc.Read(messageCnt)) return;
      if(!pc.Read(channel)) return;
      if(!pc.ReadDone()) return;
      uint16_t ret = uint16_t();
      if(mImpl) ret = mImpl->GetPwm(channel);
      pc.PacketStart(0, 101);
      pc.Write(messageCnt);
      pc.Write(ret);
      pc.PacketDone();
      break;
    }
    case 102: // ResetPwm
    {
      if(!pc.ReadDone()) return;
      if(mImpl) mImpl->ResetPwm();
      break;
    }
    default:
      break;
  }
}
}
