/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef FOOPROXY_HPP
#define FOOPROXY_HPP

#include "PacketCommunication.hpp"
#include "IFoo.hpp"

namespace erpc
{
class FooProxy : public IFoo, public PacketCommunication::Callback
{
public:
  FooProxy(PacketCommunication& packetComm);
  ~FooProxy();
  void Receive(PacketCommunication& pc);
  void DoThis(const Array& array);
  uint32_t DoThat(uint32_t i);
  PCString UpperCase(const PCString& str);
private:
  volatile bool mReceiving;
  volatile uint8_t mMessageCnt;
  volatile uint8_t mPendingFunctionId;
  volatile uint8_t mPendingMessageCnt;
  PacketCommunication& mPacketComm;
};
}

#endif /*FOOPROXY_HPP*/
