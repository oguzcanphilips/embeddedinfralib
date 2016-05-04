/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef BARPROXY_HPP
#define BARPROXY_HPP

#include "PacketCommunication.hpp"
#include "IBar.hpp"

namespace erpc
{
class BarProxy : public IBar, public PacketCommunication::Callback
{
public:
  BarProxy(PacketCommunication& packetComm);
  ~BarProxy();
  void Receive(PacketCommunication& pc);
  KeyId GetKeyId();
  void SetKeyId(const KeyId& kid);
  void SetGetKeyId(KeyId& kid);
private:
  volatile bool mReceiving;
  volatile uint8_t mMessageCnt;
  volatile uint8_t mPendingFunctionId;
  volatile uint8_t mPendingMessageCnt;
  PacketCommunication& mPacketComm;
};
}

#endif /*BARPROXY_HPP*/
