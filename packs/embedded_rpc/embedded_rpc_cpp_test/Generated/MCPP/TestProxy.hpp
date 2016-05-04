/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "ITest.hpp"

public ref class TestProxy : public ITest, public PacketCommunication::Callback
{
public:

  TestProxy(PacketCommunication^ packetComm);
  ~TestProxy();
  virtual void Receive(PacketCommunication^ pc) override;
  virtual void DoThis(Array^ array);
  virtual uint16_t DoubleOutput(uint16_t i);
  virtual bool IO_In(uint16_t value);
  virtual bool IO_Out(uint16_t% value);
  virtual void IO_InOut(uint16_t% value);
private:
  volatile bool mReceiving;
  volatile uint8_t mMessageCnt;
  volatile uint8_t mPendingFunctionId;
  volatile uint8_t mPendingMessageCnt;
  PacketCommunication^ mPacketComm;
};
