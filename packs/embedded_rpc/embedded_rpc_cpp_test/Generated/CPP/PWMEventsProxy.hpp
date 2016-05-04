/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef PWMEVENTSPROXY_HPP
#define PWMEVENTSPROXY_HPP

#include "PacketCommunication.hpp"
#include "IPWMEvents.hpp"

namespace erpc
{
class PWMEventsProxy : public IPWMEvents
{
public:
  PWMEventsProxy(PacketCommunication& packetComm);
  void PwmUpdate(uint8_t channel, uint16_t value);
private:
  PacketCommunication& mPacketComm;
};
}

#endif /*PWMEVENTSPROXY_HPP*/
