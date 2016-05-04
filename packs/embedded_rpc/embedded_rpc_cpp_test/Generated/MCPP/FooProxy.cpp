/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "FooProxy.hpp"

FooProxy::FooProxy(PacketCommunication^ packetComm)
: PacketCommunication::Callback(4)
, mReceiving(false)
, mMessageCnt(0)
, mPacketComm(packetComm)
{
  mPacketComm->Register(this);
}

FooProxy::~FooProxy(){
  mPacketComm->Unregister(this);
}

void FooProxy::Receive(PacketCommunication^ pc)
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


void FooProxy::DoThis(Array^ array)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  mPacketComm->PacketStart(132, 0);
  mPacketComm->Write(array);
  mPacketComm->PacketDone();
}

uint32_t FooProxy::DoThat(uint32_t i)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mReceiving = true;
  mPendingFunctionId = 1;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm->PacketStart(132, 1);
  mPacketComm->Write(mPendingMessageCnt);
  mPacketComm->Write(i);
  mPacketComm->PacketDone();
  mPacketComm->EventReceiveWait();
  uint32_t ret = uint32_t();
  if(!mReceiving)
  {
    if(
      !mPacketComm->Read(ret) ||
      !mPacketComm->ReadDone())
    {
      ret = uint32_t();
      mPacketComm->HandleReceiveError();
    }
  }
  return ret;
}

PCString^ FooProxy::UpperCase(PCString^ str)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mReceiving = true;
  mPendingFunctionId = 2;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm->PacketStart(132, 2);
  mPacketComm->Write(mPendingMessageCnt);
  mPacketComm->Write(str);
  mPacketComm->PacketDone();
  mPacketComm->EventReceiveWait();
  PCString^ ret = gcnew PCString();
  if(!mReceiving)
  {
    if(
      !mPacketComm->Read(ret) ||
      !mPacketComm->ReadDone())
    {
      ret = gcnew PCString();
      mPacketComm->HandleReceiveError();
    }
  }
  return ret;
}
