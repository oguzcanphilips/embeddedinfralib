#ifndef PACKETCOMMUNICATIONSLIP_HPP
#define PACKETCOMMUNICATIONSLIP_HPP


#include "PacketCommunication.hpp"
using namespace System;

namespace erpc
{
    public ref class PacketCommunicationSLIP : public PacketCommunication
    {
    public:
        interface class ISerialIO
        {
        public:
            void Open();
            bool IsOpen();
            void Close();
            void Send(uint8_t v);
            bool Receive(uint8_t% v);
        };
        PacketCommunicationSLIP(ISerialIO^ serialIO);
        ~PacketCommunicationSLIP();

        virtual void Stop() override;
        virtual void ProcessReceive() override;
        virtual bool IsPacketEnded() override;
        virtual void PacketStartToken() override;
        virtual void PackedEndToken() override;
        virtual void WriteByte(uint8_t v) override;
        virtual bool ReadByte(uint8_t% v) override;
    private:
        void Send(uint8_t v);
        uint8_t ReadInternal();
        ISerialIO^ mSerialIO;
    };
}
#endif