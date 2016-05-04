/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "ITest.hpp"

public ref class TestSkeleton : public PacketCommunication::Callback
{
public:
  TestSkeleton(PacketCommunication^ packetComm);
  ~TestSkeleton();
  void SetImpl(ITest^ impl);
  virtual void Receive(PacketCommunication^ pc) override;
private:
  ITest^ mImpl;
  PacketCommunication^ mPacketComm;
};
