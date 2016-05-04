/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "IPWMEvents.hpp"

public ref class PWMEventsProxy : public IPWMEvents
{
public:

  PWMEventsProxy(PacketCommunication^ packetComm);
  ~PWMEventsProxy();
  virtual void PwmUpdate(uint8_t channel, uint16_t value);
private:
  PacketCommunication^ mPacketComm;
};
