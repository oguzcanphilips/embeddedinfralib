/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWMEventsProxy.hpp"

PWMEventsProxy::PWMEventsProxy(PacketCommunication^ packetComm)
: mPacketComm(packetComm)
{
}

PWMEventsProxy::~PWMEventsProxy(){
}

void PWMEventsProxy::PwmUpdate(uint8_t channel, uint16_t value)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  mPacketComm->PacketStart(129, 0);
  mPacketComm->Write(channel);
  mPacketComm->Write(value);
  mPacketComm->PacketDone();
}
