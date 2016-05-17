#ifndef PACKETCOMMUNICATION_HPP
#define PACKETCOMMUNICATION_HPP

#include "../PacketCommunicationConfig.h"
#include <cstdint>

namespace erpc
{
    class PacketCommunication;
    class Serialize
    {
    public:
        virtual ~Serialize(){}
        virtual void Write(PacketCommunication& packetComm) const = 0;
        virtual bool Read(PacketCommunication& packetComm) = 0;
    };

    class PacketCommunication
    {
        friend class PacketCommunicationBin;
        friend class Callback;
    public:
        class Callback
        {
            friend class PacketCommunication;
        public:
            Callback(uint8_t interfaceId) : mInterfaceId(interfaceId), registerMask(0), mNext(0){}
            virtual ~Callback()
            {
                PacketCommunication::Remove(this);
            }
            virtual void Receive(PacketCommunication& pc) = 0;
        private:
            const uint8_t mInterfaceId;
            uint32_t registerMask;
            Callback* mNext;
        };
    private:
        PacketCommunication();
    public:
        virtual ~PacketCommunication();
        void Link(PacketCommunication& link);
        void Register(Callback& callback);
        void Unregister(Callback& callback);

        virtual void ProcessReceive() = 0;
        virtual bool IsPacketEnded() = 0;
        virtual void PacketStartToken() = 0;
        virtual void PackedEndToken() = 0;
        virtual void HandleReceiveError();

        virtual void PacketStart(uint8_t interfaceId, uint8_t functionId) = 0;
        virtual void PacketDone() = 0;
        virtual bool ReadDone() = 0;

        virtual void WriteByte(uint8_t data) = 0;
        virtual bool ReadByte(uint8_t& v) = 0;

        virtual void Write(uint8_t v) = 0;
        virtual void Write(uint16_t v) = 0;
        virtual void Write(uint32_t v) = 0;
        virtual void Write(int8_t v) = 0;
        virtual void Write(int16_t v) = 0;
        virtual void Write(int32_t v) = 0;
        virtual void Write(bool v) = 0;
        virtual void Write(const Serialize& obj) = 0;
        virtual void Write(const uint8_t* data, uint16_t len) = 0;

        virtual bool Read(uint8_t& v) = 0;
        virtual bool Read(uint16_t& v) = 0;
        virtual bool Read(uint32_t& v) = 0;
        virtual bool Read(int8_t& v) = 0;
        virtual bool Read(int16_t& v) = 0;
        virtual bool Read(int32_t& v) = 0;
        virtual bool Read(bool& v) = 0;
        virtual bool Read(Serialize& obj) = 0;
        virtual bool Read(uint8_t* data, uint16_t len) = 0;

        class FunctionScope
        {
            PacketCommunication& mPc;
        public:
            FunctionScope(PacketCommunication& pc) : mPc(pc)
            {
                mPc.FunctionLock();
            }
            ~FunctionScope()
            {
                mPc.FunctionUnlock();
            }
        };
        class ReceiveScope
        {
            PacketCommunication& mPc;
        public:
            ReceiveScope(PacketCommunication& pc) : mPc(pc)
            {
                mPc.EventReceiveReset();
            }
            ~ReceiveScope()
            {
                mPc.EventReceiveDoneSet();
            }
        };
        virtual void FunctionLock(){}
        virtual void FunctionUnlock(){}

        virtual void EventReceiveSet(){}
        virtual void EventReceiveReset(){}
        virtual void EventReceiveWait(){}
        virtual void EventReceiveDoneSet(){}
        virtual void EventReceiveDoneReset(){}
        virtual void EventReceiveDoneWait(){}
    protected:
        void Receive();
        virtual bool ReadInterfaceId(uint8_t& id) = 0;
    private:
        static void Remove(Callback* callback);

        static uint32_t UsedIdMasks;
        static uint32_t ObtainIdMask();
        static void ReleaseIdMask(uint32_t idMask);
        static Callback* Callbacks;

        const uint32_t mIdMask;

        PacketCommunication* mLink;
    };
}
#endif
