/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "IFoo.hpp"

public ref class FooProxy : public IFoo, public PacketCommunication::Callback
{
public:

  FooProxy(PacketCommunication^ packetComm);
  ~FooProxy();
  virtual void Receive(PacketCommunication^ pc) override;
  virtual void DoThis(Array^ array);
  virtual uint32_t DoThat(uint32_t i);
  virtual PCString^ UpperCase(PCString^ str);
private:
  volatile bool mReceiving;
  volatile uint8_t mMessageCnt;
  volatile uint8_t mPendingFunctionId;
  volatile uint8_t mPendingMessageCnt;
  PacketCommunication^ mPacketComm;
};
