/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "IBar.hpp"

public ref class BarProxy : public IBar, public PacketCommunication::Callback
{
public:

  BarProxy(PacketCommunication^ packetComm);
  ~BarProxy();
  virtual void Receive(PacketCommunication^ pc) override;
  virtual KeyId^ GetKeyId();
  virtual void SetKeyId(KeyId^ kid);
  virtual void SetGetKeyId(KeyId^ kid);
private:
  volatile bool mReceiving;
  volatile uint8_t mMessageCnt;
  volatile uint8_t mPendingFunctionId;
  volatile uint8_t mPendingMessageCnt;
  PacketCommunication^ mPacketComm;
};
