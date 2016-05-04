/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef BARSKELETON_HPP
#define BARSKELETON_HPP

#include "PacketCommunication.hpp"
#include "IBar.hpp"

namespace erpc
{
class BarSkeleton : public PacketCommunication::Callback
{
public:
  BarSkeleton();
  ~BarSkeleton();

  void SetImpl(IBar& impl);
  void Receive(PacketCommunication& pc);
private:
  IBar* mImpl;
};
}

#endif /*BARSKELETON_HPP*/
