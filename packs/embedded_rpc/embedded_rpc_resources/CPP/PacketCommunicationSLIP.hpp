#ifndef PACKETCOMMUNICATIONSLIP_HPP
#define PACKETCOMMUNICATIONSLIP_HPP

/* RFC 1055 */
#include "PacketCommunication.hpp"
namespace erpc
{
    class ISerialIO
    {
    public:
        virtual void Send(uint8_t v) = 0;
        virtual bool Receive(uint8_t& v) = 0;
    };

    class PacketCommunicationSLIP
        : public PacketCommunication
    {
    public:
        PacketCommunicationSLIP(ISerialIO& serialIO);

        virtual void ProcessReceive() override;
        virtual bool IsPacketEnded() override;
        virtual void PacketStart() override;
        virtual void PacketEnd() override;
        virtual void WriteByte(uint8_t data) override;
        virtual bool ReadByte(uint8_t& v) override;

    private:
        uint8_t ReadInternal();
        ISerialIO& mSerialIO;
    };
}
#endif
