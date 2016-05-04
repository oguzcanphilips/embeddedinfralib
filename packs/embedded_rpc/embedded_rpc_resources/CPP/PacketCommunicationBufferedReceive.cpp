#include "PacketCommunicationBufferedReceive.hpp"
#include <cassert>
#include <cstdlib>

#ifndef UINT32_MAX
#define UINT32_MAX 4294967295U
#endif

namespace erpc
{
    PacketCommunicationBufferedReceive::CyclicBuffer::CyclicBuffer(uint8_t* aBufferStart, uint8_t* aBufferEnd)
        : bufferStart(aBufferStart)
        , bufferEnd(aBufferEnd)
        , begin(bufferStart)
        , end(bufferStart)
    {
        assert((bufferEnd - bufferStart) <= UINT32_MAX);
    }

    bool PacketCommunicationBufferedReceive::CyclicBuffer::Empty() const
    {
        return begin == end;
    }

    bool PacketCommunicationBufferedReceive::CyclicBuffer::Full() const
    {
        return Cyclicize(Next(end)) == begin;
    }

    uint32_t PacketCommunicationBufferedReceive::CyclicBuffer::Size() const
    {
        if (begin <= end)
            return uint32_t(end - begin);
        else
            return uint32_t((end - bufferStart) + (bufferEnd - begin));
    }

    uint32_t PacketCommunicationBufferedReceive::CyclicBuffer::MaxSize() const
    {
        return uint32_t(bufferEnd - bufferStart - 1);
    }

    uint8_t& PacketCommunicationBufferedReceive::CyclicBuffer::Front()
    {
        assert(!Empty());

        return *begin;
    }

    const uint8_t& PacketCommunicationBufferedReceive::CyclicBuffer::Front() const
    {
        assert(!Empty());

        return *begin;
    }

    uint8_t& PacketCommunicationBufferedReceive::CyclicBuffer::Back()
    {
        assert(!Empty());

        return *Cyclicize(Prev(end));
    }

    const uint8_t& PacketCommunicationBufferedReceive::CyclicBuffer::Back() const
    {
        assert(!Empty());

        return *Cyclicize(Prev(end));
    }

    void PacketCommunicationBufferedReceive::CyclicBuffer::PushBack(uint8_t data)
    {
        assert(!Full());

        *end = data;
        end = Cyclicize(Next(end));
    }

    uint8_t PacketCommunicationBufferedReceive::CyclicBuffer::PopBack()
    {
        assert(!Empty());

        end = Cyclicize(Prev(end));
        return *end;
    }

    uint8_t PacketCommunicationBufferedReceive::CyclicBuffer::PopFront()
    {
        assert(!Empty());

        uint8_t data = *begin;
        begin = Cyclicize(Next(begin));

        return data;
    }

    void PacketCommunicationBufferedReceive::CyclicBuffer::PopBackNum(uint32_t num)
    {
        assert(Size() >= num);

        end = Cyclicize(end - num);
    }

    uint8_t* PacketCommunicationBufferedReceive::CyclicBuffer::Cyclicize(uint8_t* p) const
    {
        uint32_t bufferSize = uint32_t(bufferEnd - bufferStart);

        return bufferStart + ((p - bufferStart) + bufferSize) % bufferSize;
    }

    uint8_t* PacketCommunicationBufferedReceive::CyclicBuffer::Prev(uint8_t* current) const
    {
        return current - 1;
    }

    uint8_t* PacketCommunicationBufferedReceive::CyclicBuffer::Next(uint8_t* current) const
    {
        return current + 1;
    }

    PacketCommunicationBufferedReceive::PacketCommunicationBufferedReceive(uint8_t* bufferStart, uint8_t* bufferEnd)
        : buffer(bufferStart, bufferEnd)
        , receivingFront(bufferStart)
        , waitingForStart(true)
        , readingLength(0)
        , numRead(0)
    {}

    void PacketCommunicationBufferedReceive::ReceivedStart()
    {
        if (!waitingForStart)
        {
            buffer.PopBackNum(currentReceivingLength + 4);
            waitingForStart = true;
            currentReceivingLength = 0;
        }

        if (buffer.MaxSize() - buffer.Size() < 4)
        {
            BufferFull();
        }
        else
        {
            buffer.PushBack(0);
            buffer.PushBack(0);
            buffer.PushBack(0);
            buffer.PushBack(0);

            currentReceivingLength = 0;
            waitingForStart = false;
        }
    }

    void PacketCommunicationBufferedReceive::ReceivedData(uint8_t data)
    {
        if (!waitingForStart && buffer.Full())
        {
            BufferFull();

            if (buffer.Full())
            {
                buffer.PopBackNum(currentReceivingLength + 4);
                waitingForStart = true;
            }
        }

        if (!waitingForStart)
        {
            buffer.PushBack(data);
            ++currentReceivingLength;
        }
    }

    void PacketCommunicationBufferedReceive::ReceivedEnd()
    {
        if (!waitingForStart)
        {
            *receivingFront = currentReceivingLength & 0xFF;
            *buffer.Cyclicize(receivingFront + 1) = (currentReceivingLength >> 8) & 0xFF;
            *buffer.Cyclicize(receivingFront + 2) = (currentReceivingLength >> 16) & 0xFF;
            *buffer.Cyclicize(receivingFront + 3) = (currentReceivingLength >> 24) & 0xFF;
            receivingFront = buffer.Cyclicize(receivingFront + currentReceivingLength + 4);
            waitingForStart = true;

            ProcessReceive();
        }
    }

    void PacketCommunicationBufferedReceive::BufferFull()
    {
        std::abort();
    }

    void PacketCommunicationBufferedReceive::FindNextMessageStart()
    {
        uint8_t data;
        while (ReadByte(data))
        {
        }

        numRead = 0;
    }

    void PacketCommunicationBufferedReceive::ProcessReceive()
    {
        while (PacketAvailable())
        {
            Receive();
            FindNextMessageStart();
        }
    }

    bool PacketCommunicationBufferedReceive::IsPacketEnded()
    {
        return numRead == readingLength;
    }

    bool PacketCommunicationBufferedReceive::PacketAvailable() const
    {
        return !(buffer.Empty() || &buffer.Front() == receivingFront);
    }

    bool PacketCommunicationBufferedReceive::ReadByte(uint8_t& data)
    {
        if (!PacketAvailable())
            return false;

        if (numRead == 0)
        {
            readingLength = buffer.PopFront();
            readingLength += static_cast<uint32_t>(buffer.PopFront()) << 8;
            readingLength += static_cast<uint32_t>(buffer.PopFront()) << 16;
            readingLength += static_cast<uint32_t>(buffer.PopFront()) << 24;
        }

        if (numRead == readingLength)
            return false;

        data = buffer.PopFront();
        ++numRead;
        return true;
    }
}