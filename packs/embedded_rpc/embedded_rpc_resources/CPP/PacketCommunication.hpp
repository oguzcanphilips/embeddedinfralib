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

        PacketCommunication();
        virtual ~PacketCommunication();
        void Link(PacketCommunication& link);
        void Register(Callback& callback);
        void Unregister(Callback& callback);

        virtual void ProcessReceive() = 0;
        virtual bool IsPacketEnded() = 0;
        virtual void PacketStart() = 0;
        virtual void PacketEnd() = 0;
        virtual void HandleReceiveError();

        void PacketStart(uint8_t interfaceId, uint8_t functionId);
        void PacketDone();
        bool ReadDone();

        virtual void WriteByte(uint8_t data) = 0;
        virtual bool ReadByte(uint8_t& v) = 0;

        void Write(uint8_t v);
        void Write(uint16_t v);
        void Write(uint32_t v);
        void Write(int8_t v);
        void Write(int16_t v);
        void Write(int32_t v);
        void Write(bool v);
        void Write(const Serialize& obj);
        void Write(const uint8_t* data, uint16_t len);

        bool Read(uint8_t& v);
        bool Read(uint16_t& v);
        bool Read(uint32_t& v);
        bool Read(int8_t& v);
        bool Read(int16_t& v);
        bool Read(int32_t& v);
        bool Read(bool& v);
        bool Read(Serialize& obj);
        bool Read(uint8_t* data, uint16_t len);

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
    private:
        static void Remove(Callback* callback);
        void WriteInternal(uint8_t v);
        bool ReadInternal(uint8_t& v);

        static uint32_t UsedIdMasks;
        static uint32_t ObtainIdMask();
        static void ReleaseIdMask(uint32_t idMask);
        static Callback* Callbacks;

        const uint32_t mIdMask;

#ifdef VALIDATION_CRC
        uint16_t mCrcWr;
        uint16_t mCrcRd;
#endif
#ifdef VALIDATION_CHECKSUM
        uint8_t mChecksumWr;
        uint8_t mChecksumRd;
#endif
        PacketCommunication* mLink;
    };
}
#endif
