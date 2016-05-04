/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "IPWM.hpp"
#include "PWMEventsProxy.hpp"

public ref class PWMSkeleton : public PacketCommunication::Callback
{
public:
  PWMSkeleton(PacketCommunication^ packetComm);
  ~PWMSkeleton();
  IPWMEvents^ GetEventsInterface();
  void SetImpl(IPWM^ impl);
  virtual void Receive(PacketCommunication^ pc) override;
private:
  IPWM^ mImpl;
  PacketCommunication^ mPacketComm;
  PWMEventsProxy^ mEventsProxy;
};
