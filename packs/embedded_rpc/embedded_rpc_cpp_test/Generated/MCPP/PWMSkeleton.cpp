/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWMSkeleton.hpp"

PWMSkeleton::PWMSkeleton(PacketCommunication^ packetComm)
: Callback(128) // Interface Id
, mPacketComm(packetComm)
, mEventsProxy(gcnew PWMEventsProxy(packetComm))
{
  mPacketComm->Register(this);
}

PWMSkeleton::~PWMSkeleton()
{
  if (mImpl)
  {
    mImpl->PwmUpdateEvent -= gcnew IPWM::PwmUpdateDelegate(mEventsProxy, &PWMEventsProxy::PwmUpdate);
  }
  mPacketComm->Unregister(this);
}

void PWMSkeleton::SetImpl(IPWM^ impl)
{
  mImpl = impl;
  mImpl->PwmUpdateEvent += gcnew IPWM::PwmUpdateDelegate(mEventsProxy, &PWMEventsProxy::PwmUpdate);
}

IPWMEvents^ PWMSkeleton::GetEventsInterface()
{
  return mEventsProxy;
}

void PWMSkeleton::Receive(PacketCommunication^ pc)
{
  uint8_t func;
  if(!pc->Read(func)) return;
  switch(func)
  {
    case 100:
    {
      uint8_t channel = uint8_t();
      uint16_t value = uint16_t();
      if(!pc->Read(channel)) return;
      if(!pc->Read(value)) return;
      if(!pc->ReadDone()) return;
      if(mImpl) mImpl->SetPwm(channel, value);
      break;
    }
    case 101:
    {
      uint8_t channel = uint8_t();
      uint8_t messageCnt;
      if(!pc->Read(messageCnt)) return;
      if(!pc->Read(channel)) return;
      if(!pc->ReadDone()) return;
      uint16_t ret = uint16_t();
      if(mImpl) ret = mImpl->GetPwm(channel);
      pc->PacketStart(0, 101);
      pc->Write(messageCnt);
      pc->Write(ret);
      pc->PacketDone();
      break;
    }
    case 102:
    {
      if(!pc->ReadDone()) return;
      if(mImpl) mImpl->ResetPwm();
      break;
    }
    default:
      break;
  }
}
