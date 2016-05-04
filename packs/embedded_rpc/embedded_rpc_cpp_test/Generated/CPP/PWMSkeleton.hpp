/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef PWMSKELETON_HPP
#define PWMSKELETON_HPP

#include "PacketCommunication.hpp"
#include "IPWM.hpp"
#include SIGNAL_SLOT_HEADER
#include "PWMEventsProxy.hpp"

namespace erpc
{
class PWMSkeleton : public PacketCommunication::Callback
{
public:
  PWMSkeleton(PacketCommunication& packetCommEvents);
  ~PWMSkeleton();

  SLOT<PWMEventsProxy, uint8_t/*channel*/, uint16_t/*value*/> PwmUpdate;

  void SetImpl(IPWM& impl);
  void Receive(PacketCommunication& pc);
private:
  IPWM* mImpl;
  PWMEventsProxy mEventsProxy;
};
}

#endif /*PWMSKELETON_HPP*/
