/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "TestProxy.hpp"

TestProxy::TestProxy(PacketCommunication^ packetComm)
: PacketCommunication::Callback(2)
, mReceiving(false)
, mMessageCnt(0)
, mPacketComm(packetComm)
{
  mPacketComm->Register(this);
}

TestProxy::~TestProxy(){
  mPacketComm->Unregister(this);
}

void TestProxy::Receive(PacketCommunication^ pc)
{
  uint8_t functionId;
  if(!pc->Read(functionId)) return;
  if(mPendingFunctionId != functionId) return;
  uint8_t messageCnt;
  if(!pc->Read(messageCnt)) return;
  if(mPendingMessageCnt != messageCnt) return;
  mReceiving = false;
  pc->EventReceiveDoneReset();
  pc->EventReceiveSet();
  pc->EventReceiveDoneWait();
}


void TestProxy::DoThis(Array^ array)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  mPacketComm->PacketStart(130, 0);
  mPacketComm->Write(array);
  mPacketComm->PacketDone();
}

uint16_t TestProxy::DoubleOutput(uint16_t i)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mReceiving = true;
  mPendingFunctionId = 1;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm->PacketStart(130, 1);
  mPacketComm->Write(mPendingMessageCnt);
  mPacketComm->Write(i);
  mPacketComm->PacketDone();
  mPacketComm->EventReceiveWait();
  uint16_t ret = uint16_t();
  if(!mReceiving)
  {
    if(
      !mPacketComm->Read(ret) ||
      !mPacketComm->ReadDone())
    {
      ret = uint16_t();
      mPacketComm->HandleReceiveError();
    }
  }
  return ret;
}

bool TestProxy::IO_In(uint16_t value)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mReceiving = true;
  mPendingFunctionId = 2;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm->PacketStart(130, 2);
  mPacketComm->Write(mPendingMessageCnt);
  mPacketComm->Write(value);
  mPacketComm->PacketDone();
  mPacketComm->EventReceiveWait();
  bool ret = bool();
  if(!mReceiving)
  {
    if(
      !mPacketComm->Read(ret) ||
      !mPacketComm->ReadDone())
    {
      ret = bool();
      mPacketComm->HandleReceiveError();
    }
  }
  return ret;
}

bool TestProxy::IO_Out(uint16_t% value)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mReceiving = true;
  mPendingFunctionId = 3;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm->PacketStart(130, 3);
  mPacketComm->Write(mPendingMessageCnt);
  mPacketComm->PacketDone();
  mPacketComm->EventReceiveWait();
  bool ret = bool();
  if(!mReceiving)
  {
    if(
      !mPacketComm->Read(ret) ||
      !mPacketComm->Read(value) ||
      !mPacketComm->ReadDone())
    {
      ret = bool();
      value = uint16_t();
      mPacketComm->HandleReceiveError();
    }
  }
  return ret;
}

void TestProxy::IO_InOut(uint16_t% value)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mReceiving = true;
  mPendingFunctionId = 4;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm->PacketStart(130, 4);
  mPacketComm->Write(mPendingMessageCnt);
  mPacketComm->Write(value);
  mPacketComm->PacketDone();
  mPacketComm->EventReceiveWait();
  if(!mReceiving)
  {
    if(
      !mPacketComm->Read(value) ||
      !mPacketComm->ReadDone())
    {
      value = uint16_t();
      mPacketComm->HandleReceiveError();
    }
  }
}
