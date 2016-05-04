/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef PWMPROXY_HPP
#define PWMPROXY_HPP

#include "PacketCommunication.hpp"
#include "IPWM.hpp"
#include SIGNAL_SLOT_HEADER
#include "PWMEventsSkeleton.hpp"

namespace erpc
{
class PWMProxy : public IPWM, private IPWMEvents, public PacketCommunication::Callback
{
public:
  PWMProxy(PacketCommunication& packetComm);
  ~PWMProxy();
  void Receive(PacketCommunication& pc);
  void SetPwm(uint8_t channel, uint16_t value);
  uint16_t GetPwm(uint8_t channel);
  void ResetPwm();
private:
  void PwmUpdate(uint8_t channel, uint16_t value){IPWM::PwmUpdate(channel, value);}
  volatile bool mReceiving;
  volatile uint8_t mMessageCnt;
  volatile uint8_t mPendingFunctionId;
  volatile uint8_t mPendingMessageCnt;
  PacketCommunication& mPacketComm;
  PWMEventsSkeleton mEventsSkeleton;
};
}

#endif /*PWMPROXY_HPP*/
