/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "TestProxy.hpp"

namespace erpc
{
TestProxy::TestProxy(PacketCommunication& packetComm)
: PacketCommunication::Callback(2)
, mReceiving(false)
, mMessageCnt(0)
, mPacketComm(packetComm)
{
  mPacketComm.Register(*this);
}

TestProxy::~TestProxy()
{
  mPacketComm.Unregister(*this);
}

void TestProxy::Receive(PacketCommunication& pc)
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


void TestProxy::DoThis(const Array& array)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mPacketComm.PacketStart(130, 0);
  mPacketComm.Write(array);
  mPacketComm.PacketDone();
}

uint16_t TestProxy::DoubleOutput(uint16_t i)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mPacketComm.EventReceiveReset();
  mReceiving = true;
  mPendingFunctionId = 1;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm.PacketStart(130, 1);
  mPacketComm.Write(mPendingMessageCnt);
  mPacketComm.Write(i);
  mPacketComm.PacketDone();
  mPacketComm.EventReceiveWait();
  if(mReceiving) mPacketComm.ProcessReceive();
  uint16_t ret = uint16_t();
  if(mReceiving
    || !mPacketComm.Read(ret)
    || !mPacketComm.ReadDone())
  {
    mPacketComm.HandleReceiveError();
    ret = uint16_t();
  }
  return ret;
}

bool TestProxy::IO_In(uint16_t value)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mPacketComm.EventReceiveReset();
  mReceiving = true;
  mPendingFunctionId = 2;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm.PacketStart(130, 2);
  mPacketComm.Write(mPendingMessageCnt);
  mPacketComm.Write(value);
  mPacketComm.PacketDone();
  mPacketComm.EventReceiveWait();
  if(mReceiving) mPacketComm.ProcessReceive();
  bool ret = bool();
  if(mReceiving
    || !mPacketComm.Read(ret)
    || !mPacketComm.ReadDone())
  {
    mPacketComm.HandleReceiveError();
    ret = bool();
  }
  return ret;
}

bool TestProxy::IO_Out(uint16_t& value)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mPacketComm.EventReceiveReset();
  mReceiving = true;
  mPendingFunctionId = 3;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm.PacketStart(130, 3);
  mPacketComm.Write(mPendingMessageCnt);
  mPacketComm.PacketDone();
  mPacketComm.EventReceiveWait();
  if(mReceiving) mPacketComm.ProcessReceive();
  bool ret = bool();
  if(mReceiving
    || !mPacketComm.Read(ret)
    || !mPacketComm.Read(value)
    || !mPacketComm.ReadDone())
  {
    mPacketComm.HandleReceiveError();
    ret = bool();
    value = uint16_t();
  }
  return ret;
}

void TestProxy::IO_InOut(uint16_t& value)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mPacketComm.EventReceiveReset();
  mReceiving = true;
  mPendingFunctionId = 4;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm.PacketStart(130, 4);
  mPacketComm.Write(mPendingMessageCnt);
  mPacketComm.Write(value);
  mPacketComm.PacketDone();
  mPacketComm.EventReceiveWait();
  if(mReceiving) mPacketComm.ProcessReceive();
  if(mReceiving
    || !mPacketComm.Read(value)
    || !mPacketComm.ReadDone())
  {
    mPacketComm.HandleReceiveError();
    value = uint16_t();
  }
}
}
