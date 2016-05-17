#ifndef PACKETCOMMUNICATIONSLIP_HPP
#define PACKETCOMMUNICATIONSLIP_HPP

/* RFC 1055 */
#include "PacketCommunicationBin.hpp"
namespace erpc
{
    class ISerialIO
    {
    public:
        virtual void Send(uint8_t v) = 0;
        virtual bool Receive(uint8_t& v) = 0;
    };

    class PacketCommunicationSLIP
        : public PacketCommunicationBin
    {
    public:
        PacketCommunicationSLIP(ISerialIO& serialIO);

        virtual void ProcessReceive() override;
        virtual bool IsPacketEnded() override;
        virtual void PacketStartToken() override;
        virtual void PackedEndToken() override;
        virtual void WriteByte(uint8_t data) override;
        virtual bool ReadByte(uint8_t& v) override;

    private:
        uint8_t ReadInternal();
        ISerialIO& mSerialIO;
    };
}
#endif
