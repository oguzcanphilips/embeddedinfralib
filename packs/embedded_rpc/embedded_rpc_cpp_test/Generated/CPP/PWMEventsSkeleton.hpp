/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef PWMEVENTSSKELETON_HPP
#define PWMEVENTSSKELETON_HPP

#include "PacketCommunication.hpp"
#include "IPWMEvents.hpp"

namespace erpc
{
class PWMEventsSkeleton : public PacketCommunication::Callback
{
public:
  PWMEventsSkeleton();
  ~PWMEventsSkeleton();

  void SetImpl(IPWMEvents& impl);
  void Receive(PacketCommunication& pc);
private:
  IPWMEvents* mImpl;
};
}

#endif /*PWMEVENTSSKELETON_HPP*/
