#ifndef PACKETCOMMUNICATION_HPP
#define PACKETCOMMUNICATION_HPP

using namespace System::Collections::Generic;
using namespace System::Threading;

#include "../PacketCommunicationConfig.h"
#include <stdint.h>

namespace erpc
{
    ref class PacketCommunication;
    public interface class Serialize
    {
    public:
        void Write(PacketCommunication^ packetComm);
        bool Read(PacketCommunication^ packetComm);
    };

    public ref class PacketCommunication abstract
    {
    public:
        delegate void ErrorOccuredDelegate();
        event ErrorOccuredDelegate^ ErrorOccuredEvent;

        ref class Callback abstract
        {
        public:
            Callback(uint8_t interfaceId) : InterfaceId(interfaceId){}
            virtual ~Callback(){}
            virtual void Receive(PacketCommunication^ pc) = 0;
            const uint8_t InterfaceId;
        };

        PacketCommunication();
        virtual ~PacketCommunication();

        virtual void Stop();

        void Register(Callback^ callback);
        void Unregister(Callback^ callback);

        virtual void ProcessReceive() = 0;
        virtual bool IsPacketEnded() = 0;
        virtual void PacketStart() = 0;
        virtual void PacketEnd() = 0;
        bool ReadDone();

        void PacketStart(uint8_t interfaceId, uint8_t functionId);
        void PacketDone();
        virtual void WriteByte(uint8_t v) = 0;
        void Write(uint8_t v);
        void Write(uint16_t v);
        void Write(uint32_t v);
        void Write(int8_t v)     { Write((uint8_t)v); }
        void Write(int16_t v)    { Write((uint16_t)v); }
        void Write(int32_t v)    { Write((uint32_t)v); }
        void Write(bool v)       { Write((uint8_t)(v ? 1 : 0)); }
        void Write(Serialize^ obj) { obj->Write(this); }
        void Write(const uint8_t* data, uint32_t len);

        virtual bool ReadByte(uint8_t% v) = 0;
        bool Read(uint8_t% v);
        bool Read(uint16_t% v);
        bool Read(uint32_t% v);
        bool Read(int8_t% v)     { return Read((uint8_t%)v); }
        bool Read(int16_t% v)    { return Read((uint16_t%)v); }
        bool Read(int32_t% v)    { return Read((uint32_t%)v); }
        bool Read(uint8_t* data, uint32_t len);
        bool Read(bool% v);
        bool Read(Serialize^ obj){ return obj->Read(this); }

        virtual void HandleReceiveError();

        ref class FunctionScope
        {
            PacketCommunication^ mPc;
        public:
            FunctionScope(PacketCommunication^ pc) : mPc(pc)
            {
                mPc->FunctionLock();
            }
            ~FunctionScope()
            {
                mPc->FunctionUnlock();
            }
        };
        ref class ReceiveScope
        {
            PacketCommunication^ mPc;
        public:
            ReceiveScope(PacketCommunication^ pc) : mPc(pc)
            {
                mPc->EventReceiveReset();
            }
            ~ReceiveScope()
            {
                mPc->EventReceiveDoneSet();
            }
        };
        void FunctionLock();
        void FunctionUnlock();

        void EventReceiveSet();
        void EventReceiveReset();
        void EventReceiveWait();
        void EventReceiveDoneSet();
        void EventReceiveDoneReset();
        void EventReceiveDoneWait();
    protected:
        void Start();
        void Receive();
        bool IsStarted() { return mRun; }
    private:
        void ReceiveThread();
        bool ReadInternal(uint8_t% v);
        void WriteInternal(uint8_t v);

        Dictionary<uint8_t, Callback^>^ mCallbacks;
        EventWaitHandle^ mReceiveEvent;
        EventWaitHandle^ mReceiveDoneEvent;
        Mutex^ mFunctionLock;
        Thread^ mReceiveThread;
        bool mRun;
#ifdef VALIDATION_CRC
        uint16_t mCrcWr;
        uint16_t mCrcRd;
#endif
#ifdef VALIDATION_CHECKSUM
        uint8_t mChecksumWr;
        uint8_t mChecksumRd;
#endif
    };
}
#endif
