/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "IBar.hpp"

public ref class BarSkeleton : public PacketCommunication::Callback
{
public:
  BarSkeleton(PacketCommunication^ packetComm);
  ~BarSkeleton();
  void SetImpl(IBar^ impl);
  virtual void Receive(PacketCommunication^ pc) override;
private:
  IBar^ mImpl;
  PacketCommunication^ mPacketComm;
};
