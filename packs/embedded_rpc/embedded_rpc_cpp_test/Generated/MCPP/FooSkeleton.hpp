/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "IFoo.hpp"

public ref class FooSkeleton : public PacketCommunication::Callback
{
public:
  FooSkeleton(PacketCommunication^ packetComm);
  ~FooSkeleton();
  void SetImpl(IFoo^ impl);
  virtual void Receive(PacketCommunication^ pc) override;
private:
  IFoo^ mImpl;
  PacketCommunication^ mPacketComm;
};
