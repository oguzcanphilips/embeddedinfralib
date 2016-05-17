#ifndef BUFFERED_PACKET_COMMUNICATION_HPP
#define BUFFERED_PACKET_COMMUNICATION_HPP

#include "PacketCommunicationBin.hpp"

namespace erpc
{
    class PacketCommunicationBufferedReceive
        : public PacketCommunicationBin
    {
    public:
        class CyclicBuffer
        {
        public:
            CyclicBuffer(uint8_t* aBufferStart, uint8_t* aBufferEnd);

            bool Empty() const;
            bool Full() const;
            uint32_t Size() const;
            uint32_t MaxSize() const;

            uint8_t& Front();
            const uint8_t& Front() const;
            uint8_t& Back();
            const uint8_t& Back() const;

            void PushBack(uint8_t data);
            uint8_t PopBack();
            uint8_t PopFront();

            void PopBackNum(uint32_t num);

            uint8_t* Cyclicize(uint8_t* p) const;

        private:
            uint8_t* Prev(uint8_t* current) const;
            uint8_t* Next(uint8_t* current) const;

            uint8_t* bufferStart;
            uint8_t* bufferEnd;
            uint8_t* begin;
            uint8_t* end;   // begin == end => buffer is empty
        };
        PacketCommunicationBufferedReceive(uint8_t* bufferStart, uint8_t* bufferEnd);

        void ReceivedStart();
        void ReceivedData(uint8_t data);
        void ReceivedEnd();

        virtual void BufferFull();  // Default behaviour: abort. Override to include own handling

        virtual void ProcessReceive();
        virtual bool IsPacketEnded();
        virtual bool ReadByte(uint8_t& data);

    protected:
        void FindNextMessageStart();

    private:
        bool PacketAvailable() const;

        CyclicBuffer buffer;

        uint8_t* receivingFront;
        uint32_t currentReceivingLength;
        bool waitingForStart;

        uint32_t readingLength;
        uint32_t numRead;
    };
}
#endif
