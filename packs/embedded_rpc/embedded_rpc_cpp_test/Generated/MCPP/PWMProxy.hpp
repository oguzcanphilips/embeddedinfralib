/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "IPWM.hpp"
#include "PWMEventsSkeleton.hpp"

public ref class PWMProxy : public IPWM, public IPWMEvents, public PacketCommunication::Callback
{
public:
  virtual event IPWM::PwmUpdateDelegate^ PwmUpdateEvent;
  virtual void PwmUpdate(uint8_t channel, uint16_t value){ PwmUpdateEvent(channel, value);}

  PWMProxy(PacketCommunication^ packetComm);
  ~PWMProxy();
  virtual void Receive(PacketCommunication^ pc) override;
  virtual void SetPwm(uint8_t channel, uint16_t value);
  virtual uint16_t GetPwm(uint8_t channel);
  virtual void ResetPwm();
private:
  volatile bool mReceiving;
  volatile uint8_t mMessageCnt;
  volatile uint8_t mPendingFunctionId;
  volatile uint8_t mPendingMessageCnt;
  PacketCommunication^ mPacketComm;
  PWMEventsSkeleton^ mEventsSkeleton;
};
