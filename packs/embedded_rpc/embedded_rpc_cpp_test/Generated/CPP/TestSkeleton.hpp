/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef TESTSKELETON_HPP
#define TESTSKELETON_HPP

#include "PacketCommunication.hpp"
#include "ITest.hpp"

namespace erpc
{
class TestSkeleton : public PacketCommunication::Callback
{
public:
  TestSkeleton();
  ~TestSkeleton();

  void SetImpl(ITest& impl);
  void Receive(PacketCommunication& pc);
private:
  ITest* mImpl;
};
}

#endif /*TESTSKELETON_HPP*/
