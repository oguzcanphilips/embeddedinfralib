#include "PacketCommunication.h"

#ifdef VALIDATION_CRC
#include "../CRC.h"
#endif

PacketCommunication::PacketCommunication()
    : mCallbacks(gcnew Dictionary<uint8_t, Callback^>)
    , mReceiveEvent(gcnew EventWaitHandle(false, EventResetMode::ManualReset))
    , mReceiveDoneEvent(gcnew EventWaitHandle(false, EventResetMode::ManualReset))
    , mFunctionLock(gcnew Mutex(false))
    , mRun(false)
#ifdef VALIDATION_CRC
    , mCrcWr(0)
    , mCrcRd(0)
#endif
#ifdef VALIDATION_CHECKSUM
    , mChecksumWr(0)
    , mChecksumRd(0)
#endif
{
    mReceiveThread = gcnew Thread(gcnew ThreadStart(this, &PacketCommunication::ReceiveThread));
}

PacketCommunication::~PacketCommunication()
{
    Stop();
}

void PacketCommunication::ReceiveThread()
{
    while(mRun)
    {
        try
        {
            ProcessReceive();
        }
        catch(...)
        {
        }
    }
}

void PacketCommunication::Register(Callback^ callback)
{
    if(mCallbacks->ContainsKey(callback->InterfaceId)) 
    {
        throw gcnew System::Exception("Callback already registered");
    }
    mCallbacks->Add(callback->InterfaceId, callback);
}

void PacketCommunication::Unregister(Callback^ callback)
{
    mCallbacks->Remove(callback->InterfaceId);
}

void PacketCommunication::Receive()
{
    uint8_t interfaceId;
#ifdef VALIDATION_CRC
  mCrcRd = 0;
#endif
#ifdef VALIDATION_CHECKSUM
  mChecksumRd = 0;
#endif
    if(!Read(interfaceId)) return;
    if(!mCallbacks->ContainsKey(interfaceId)) return;
    Callback^ cb = mCallbacks[interfaceId];
    if(cb != nullptr) cb->Receive(this);
}

void PacketCommunication::PacketStart(uint8_t interfaceId, uint8_t functionId)
{
#ifdef VALIDATION_CRC
  mCrcWr = 0;
#endif
#ifdef VALIDATION_CHECKSUM
  mChecksumWr = 0;
#endif
    PacketStart();
    WriteInternal(interfaceId);
    WriteInternal(functionId);
}

void PacketCommunication::PacketDone()
{
#ifdef VALIDATION_CRC
    WriteByte(mCrcWr >> 8);
    WriteByte(mCrcWr & 0xff);
#endif
#ifdef VALIDATION_CHECKSUM
    WriteByte(mChecksumWr);
#endif
    PacketEnd();
}

bool PacketCommunication::ReadInternal(uint8_t% v)
{
    if(!ReadByte(v))
    {
        return false;
    }
#ifdef VALIDATION_CRC
    uint16_t tmp = mCrcRd;
    CRC_Update(&tmp, v);
    mCrcRd = tmp;
#endif
#ifdef VALIDATION_CHECKSUM
    mChecksumRd += v;
#endif
    return true;
}

void PacketCommunication::WriteInternal(uint8_t v)
{
#ifdef VALIDATION_CRC
    uint16_t tmp = mCrcWr;
    CRC_Update(&tmp, v);
    mCrcWr= tmp;
#endif
#ifdef VALIDATION_CHECKSUM
    mChecksumWr += v;
#endif
    WriteByte(v);
}

void PacketCommunication::Write(uint8_t v)
{
    WriteInternal(v);
}

void PacketCommunication::Write(uint16_t v)
{
    WriteInternal(v >> 8);
    WriteInternal(v & 0xff);
}

void PacketCommunication::Write(uint32_t v)
{
    WriteInternal(v >> 24);
    WriteInternal((v >> 16) & 0xff);
    WriteInternal((v >> 8) & 0xff);
    WriteInternal(v& 0xff);
}

bool PacketCommunication::Read(uint8_t% v)
{
    return ReadInternal(v);
}

bool PacketCommunication::Read(uint16_t% v)
{
    uint8_t c;
    if(!ReadInternal(c)) return false;
    v = c;
    v <<= 8;
    if(!ReadInternal(c)) return false;
    v |= c;
    return true;
}

bool PacketCommunication::Read(uint32_t% v)
{
    uint8_t c;
    if(!ReadInternal(c)) return false;
    v = c;
    v <<= 8;
    if(!ReadInternal(c)) return false;
    v |= c;
    v <<= 8;
    if(!ReadInternal(c)) return false;
    v |= c;
    v <<= 8;
    if(!ReadInternal(c)) return false;
    v |= c;
    return true;
}

bool PacketCommunication::Read(bool% v)
{
    uint8_t c;
    if(!ReadInternal(c)) return false;
    v = c ? true : false;
    return true;
}

bool PacketCommunication::Read(uint8_t* data, uint16_t len)
{
    for(uint16_t i=0;i<len;++i)
    {
        if(!ReadInternal(data[i])) return false;
    }
    return true;
}

bool PacketCommunication::ReadDone()
{
#if defined(VALIDATION_CRC)
    uint8_t dummy1,dummy2;
    if(!ReadInternal(dummy1) || !ReadInternal(dummy2)) return false;
    return IsPacketEnded() && (mCrcRd == 0);
#elif defined(VALIDATION_CHECKSUM)
    uint8_t checksum;
    if(!ReadByte(checksum)) return false;
    return IsPacketEnded() && (mChecksumRd == checksum);
#else
    return IsPacketEnded();
#endif
}

void PacketCommunication::HandleReceiveError()
{
	ErrorOccuredEvent();
}

void PacketCommunication::FunctionLock()
{
    mFunctionLock->WaitOne();
}

void PacketCommunication::FunctionUnlock()
{
    mFunctionLock->ReleaseMutex();
}

void PacketCommunication::EventReceiveSet()
{
    mReceiveEvent->Set();
}

void PacketCommunication::EventReceiveReset()
{
    mReceiveEvent->Reset();
}

void PacketCommunication::EventReceiveWait()
{
    mReceiveEvent->WaitOne(RECEIVE_TIMEOUT_MS);
}

void PacketCommunication::EventReceiveDoneSet()
{
    mReceiveDoneEvent->Set();
}

void PacketCommunication::EventReceiveDoneReset()
{
    mReceiveDoneEvent->Reset();
}

void PacketCommunication::EventReceiveDoneWait()
{
    mReceiveDoneEvent->WaitOne(100000);
}

void PacketCommunication::Start()
{
    if(mRun) return;

    mRun = true;
    mReceiveThread->Start();
}

void PacketCommunication::Stop()
{
    if(!mRun) return;

    mRun = false;
    EventReceiveSet();
    EventReceiveDoneSet();
    mReceiveThread->Join();
    EventReceiveDoneReset();
    EventReceiveReset();
}