/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "BarProxy.hpp"

BarProxy::BarProxy(PacketCommunication^ packetComm)
: PacketCommunication::Callback(6)
, mReceiving(false)
, mMessageCnt(0)
, mPacketComm(packetComm)
{
  mPacketComm->Register(this);
}

BarProxy::~BarProxy(){
  mPacketComm->Unregister(this);
}

void BarProxy::Receive(PacketCommunication^ pc)
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


KeyId^ BarProxy::GetKeyId()
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mReceiving = true;
  mPendingFunctionId = 0;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm->PacketStart(134, 0);
  mPacketComm->Write(mPendingMessageCnt);
  mPacketComm->PacketDone();
  mPacketComm->EventReceiveWait();
  KeyId^ ret = gcnew KeyId();
  if(!mReceiving)
  {
    if(
      !mPacketComm->Read(ret) ||
      !mPacketComm->ReadDone())
    {
      ret = gcnew KeyId();
      mPacketComm->HandleReceiveError();
    }
  }
  return ret;
}

void BarProxy::SetKeyId(KeyId^ kid)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  mPacketComm->PacketStart(134, 1);
  mPacketComm->Write(kid);
  mPacketComm->PacketDone();
}

void BarProxy::SetGetKeyId(KeyId^ kid)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mReceiving = true;
  mPendingFunctionId = 2;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm->PacketStart(134, 2);
  mPacketComm->Write(mPendingMessageCnt);
  mPacketComm->Write(kid);
  mPacketComm->PacketDone();
  mPacketComm->EventReceiveWait();
  if(!mReceiving)
  {
    if(
      !mPacketComm->Read(kid) ||
      !mPacketComm->ReadDone())
    {
      kid = gcnew KeyId();
      mPacketComm->HandleReceiveError();
    }
  }
}
