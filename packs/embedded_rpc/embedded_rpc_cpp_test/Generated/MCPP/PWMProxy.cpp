/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "PWMProxy.hpp"

PWMProxy::PWMProxy(PacketCommunication^ packetComm)
: PacketCommunication::Callback(0)
, mReceiving(false)
, mMessageCnt(0)
, mPacketComm(packetComm)
, mEventsSkeleton(gcnew PWMEventsSkeleton(packetComm))
{
  mPacketComm->Register(this);
  mEventsSkeleton->SetImpl(this);
}

PWMProxy::~PWMProxy(){
  mPacketComm->Unregister(this);
  mEventsSkeleton->~PWMEventsSkeleton();
}

void PWMProxy::Receive(PacketCommunication^ pc)
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


void PWMProxy::SetPwm(uint8_t channel, uint16_t value)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  mPacketComm->PacketStart(128, 100);
  mPacketComm->Write(channel);
  mPacketComm->Write(value);
  mPacketComm->PacketDone();
}

uint16_t PWMProxy::GetPwm(uint8_t channel)
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  PacketCommunication::ReceiveScope receiveScope(mPacketComm);
  mReceiving = true;
  mPendingFunctionId = 101;
  mPendingMessageCnt = mMessageCnt++;
  mPacketComm->PacketStart(128, 101);
  mPacketComm->Write(mPendingMessageCnt);
  mPacketComm->Write(channel);
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

void PWMProxy::ResetPwm()
{
  PacketCommunication::FunctionScope functionScope(mPacketComm);
  mPacketComm->PacketStart(128, 102);
  mPacketComm->PacketDone();
}
