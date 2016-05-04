/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "IPWMEvents.hpp"

public ref class PWMEventsSkeleton : public PacketCommunication::Callback
{
public:
  PWMEventsSkeleton(PacketCommunication^ packetComm);
  ~PWMEventsSkeleton();
  void SetImpl(IPWMEvents^ impl);
  virtual void Receive(PacketCommunication^ pc) override;
private:
  IPWMEvents^ mImpl;
  PacketCommunication^ mPacketComm;
};
