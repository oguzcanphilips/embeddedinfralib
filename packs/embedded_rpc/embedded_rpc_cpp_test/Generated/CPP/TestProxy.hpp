/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef TESTPROXY_HPP
#define TESTPROXY_HPP

#include "PacketCommunication.hpp"
#include "ITest.hpp"

namespace erpc
{
class TestProxy : public ITest, public PacketCommunication::Callback
{
public:
  TestProxy(PacketCommunication& packetComm);
  ~TestProxy();
  void Receive(PacketCommunication& pc);
  void DoThis(const Array& array);
  uint16_t DoubleOutput(uint16_t i);
  bool IO_In(uint16_t value);
  bool IO_Out(uint16_t& value);
  void IO_InOut(uint16_t& value);
private:
  volatile bool mReceiving;
  volatile uint8_t mMessageCnt;
  volatile uint8_t mPendingFunctionId;
  volatile uint8_t mPendingMessageCnt;
  PacketCommunication& mPacketComm;
};
}

#endif /*TESTPROXY_HPP*/
