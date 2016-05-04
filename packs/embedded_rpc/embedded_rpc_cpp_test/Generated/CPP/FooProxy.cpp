/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "FooProxy.hpp"

namespace erpc
{
FooProxy::FooProxy(PacketCommunication& packetComm)
: PacketCommunication::Callback(4)
, mReceiving(false)
, mMessageCnt(0)
, mPacketComm(packetComm)
{
  mPacketComm.Register(*this);
}

FooProxy::~FooProxy()
{
  mPacketComm.Unregister(*this);
}

void FooProxy::Receive(PacketCommunication& pc)
{
  uint8_t functionId;
  if(!pc.Read(functionId)) return;
  if(mPendingFunctionId != functionId) return;
  uint8_t messageCnt;
  if(!pc.Read(messageCnt)) return;
  if(mPendingMessageCnt != messageCnt) return;
  mReceiving = false;
  pc.EventReceiveDoneReset();
  pc.EventReceiveSet();
  pc.EventReceiveDoneWait();
}


void FooProxy::DoThis(const Array& array)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mPacketComm.PacketStart(132, 0);
  mPacketComm.Write(array);
  mPacketComm.PacketDone();
}

uint32_t FooProxy::DoThat(uint32_t i)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mPacketComm.EventReceiveReset();
  mReceiving = true;
  mPendingFunctionId = 1;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm.PacketStart(132, 1);
  mPacketComm.Write(mPendingMessageCnt);
  mPacketComm.Write(i);
  mPacketComm.PacketDone();
  mPacketComm.EventReceiveWait();
  if(mReceiving) mPacketComm.ProcessReceive();
  uint32_t ret = uint32_t();
  if(mReceiving
    || !mPacketComm.Read(ret)
    || !mPacketComm.ReadDone())
  {
    mPacketComm.HandleReceiveError();
    ret = uint32_t();
  }
  return ret;
}

PCString FooProxy::UpperCase(const PCString& str)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mPacketComm.EventReceiveReset();
  mReceiving = true;
  mPendingFunctionId = 2;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm.PacketStart(132, 2);
  mPacketComm.Write(mPendingMessageCnt);
  mPacketComm.Write(str);
  mPacketComm.PacketDone();
  mPacketComm.EventReceiveWait();
  if(mReceiving) mPacketComm.ProcessReceive();
  PCString ret = PCString();
  if(mReceiving
    || !mPacketComm.Read(ret)
    || !mPacketComm.ReadDone())
  {
    mPacketComm.HandleReceiveError();
    ret = PCString();
  }
  return ret;
}
}
