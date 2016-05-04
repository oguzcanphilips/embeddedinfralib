/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef FOOSKELETON_HPP
#define FOOSKELETON_HPP

#include "PacketCommunication.hpp"
#include "IFoo.hpp"

namespace erpc
{
class FooSkeleton : public PacketCommunication::Callback
{
public:
  FooSkeleton();
  ~FooSkeleton();

  void SetImpl(IFoo& impl);
  void Receive(PacketCommunication& pc);
private:
  IFoo* mImpl;
};
}

#endif /*FOOSKELETON_HPP*/
