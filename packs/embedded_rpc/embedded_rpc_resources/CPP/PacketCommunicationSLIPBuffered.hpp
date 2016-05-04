#ifndef BUFFEREDPACKETCOMMUNICATIONSLIP_HPP
#define BUFFEREDPACKETCOMMUNICATIONSLIP_HPP

#include "PacketCommunicationBufferedReceive.hpp"

namespace erpc
{
    class PacketCommunicationSLIPBufferedReceive
        : public PacketCommunicationBufferedReceive
    {
    public:
        PacketCommunicationSLIPBufferedReceive(uint8_t* receiveBufferStart, uint8_t* receiveBufferEnd);

        void ReceivedSlipByte(uint8_t data);

        virtual void PacketStart() override;
        virtual void PacketEnd() override;
        virtual void WriteByte(uint8_t data) override;

    protected:
        virtual void WriteSlipByte(uint8_t data) = 0;

    private:
        bool escaping;
    };

    class PacketCommunicationSLIPBuffered
        : public PacketCommunicationSLIPBufferedReceive
    {
    public:
        PacketCommunicationSLIPBuffered(uint8_t* aSendBufferStart, uint8_t* aSendBufferEnd, uint8_t* receiveBufferStart, uint8_t* receiveBufferEnd);

    protected:
        virtual void WriteSlipByte(uint8_t data) override;

        virtual void StartSending(uint8_t* start, uint8_t* end) = 0;
        virtual void BufferOverflow();
        void SendDone(uint8_t* end);

    private:
        bool IsBufferFull() const;
        void TryStartSending();

    private:
        uint8_t* sendBufferStart;
        uint8_t* sendBufferEnd;
        uint8_t* sendDataStart;
        uint8_t* sendDataEnd;
        bool sending;
    };
}
#endif
