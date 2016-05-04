/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWMEventsSkeleton.hpp"

namespace erpc
{
PWMEventsSkeleton::PWMEventsSkeleton()
: Callback(129) // Interface Id
, mImpl(0)
{
}

PWMEventsSkeleton::~PWMEventsSkeleton()
{
}

void PWMEventsSkeleton::SetImpl(IPWMEvents& impl)
{
  mImpl = &impl;
}

void PWMEventsSkeleton::Receive(PacketCommunication& pc)
{
  uint8_t func;
  if(!pc.Read(func)) return;
  switch(func)
  {
    case 0: // PwmUpdate
    {
      uint8_t channel;
      uint16_t value;
      if(!pc.Read(channel)) return;
      if(!pc.Read(value)) return;
      if(!pc.ReadDone()) return;
      if(mImpl) mImpl->PwmUpdate(channel, value);
      break;
    }
    default:
      break;
  }
}
}
