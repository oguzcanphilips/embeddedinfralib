#include "PacketCommunicationSLIPBuffered.hpp"
#include "../PacketCommunicationDefines.h"
#include <cassert>
#include <cstdlib>
#include <iterator>

namespace erpc
{
    PacketCommunicationSLIPBufferedReceive::PacketCommunicationSLIPBufferedReceive(uint8_t* receiveBufferStart, uint8_t* receiveBufferEnd)
        : PacketCommunicationBufferedReceive(receiveBufferStart, receiveBufferEnd)
        , escaping(false)
    {}

    void PacketCommunicationSLIPBufferedReceive::ReceivedSlipByte(uint8_t data)
    {
        if (escaping)
        {
            switch (data)
            {
            case SLIP_ESC_BEGIN:
                ReceivedData(SLIP_BEGIN);
                break;
            case SLIP_ESC_END:
                ReceivedData(SLIP_END);
                break;
            case SLIP_ESC_ESC:
                ReceivedData(SLIP_ESC);
                break;
            default:
                break;
            }

            escaping = false;
        }
        else
        {
            switch (data)
            {
            case SLIP_BEGIN:
                ReceivedStart();
                break;
            case SLIP_END:
                ReceivedEnd();
                break;
            case SLIP_ESC:
                escaping = true;
                break;
            default:
                ReceivedData(data);
                break;
            }
        }
    }

    void PacketCommunicationSLIPBufferedReceive::PacketStart()
    {
        WriteSlipByte(SLIP_BEGIN);
    }

    void PacketCommunicationSLIPBufferedReceive::PacketEnd()
    {
        WriteSlipByte(SLIP_END);
    }

    void PacketCommunicationSLIPBufferedReceive::WriteByte(uint8_t data)
    {
        switch (data)
        {
        case SLIP_BEGIN:
            WriteSlipByte(SLIP_ESC);
            WriteSlipByte(SLIP_ESC_BEGIN);
            break;
        case SLIP_END:
            WriteSlipByte(SLIP_ESC);
            WriteSlipByte(SLIP_ESC_END);
            break;
        case SLIP_ESC:
            WriteSlipByte(SLIP_ESC);
            WriteSlipByte(SLIP_ESC_ESC);
            break;
        default:
            WriteSlipByte(data);
            break;
        }
    }

    PacketCommunicationSLIPBuffered::PacketCommunicationSLIPBuffered(uint8_t* aSendBufferStart, uint8_t* aSendBufferEnd, uint8_t* receiveBufferStart, uint8_t* receiveBufferEnd)
        : PacketCommunicationSLIPBufferedReceive(receiveBufferStart, receiveBufferEnd)
        , sendBufferStart(aSendBufferStart)
        , sendBufferEnd(aSendBufferEnd)
        , sendDataStart(aSendBufferStart)
        , sendDataEnd(aSendBufferStart)
        , sending(false)
    {}

    void PacketCommunicationSLIPBuffered::WriteSlipByte(uint8_t data)
    {
        if (IsBufferFull())
            BufferOverflow();

        if (!IsBufferFull())
        {
            *sendDataEnd++ = data;

            if (sendDataEnd == sendBufferEnd)
                sendDataEnd = sendBufferStart;

            TryStartSending();
        }
    }

    void PacketCommunicationSLIPBuffered::BufferOverflow()
    {
        assert(false);

        while (IsBufferFull())
        {
        }
    }

    bool PacketCommunicationSLIPBuffered::IsBufferFull() const
    {
        return sendDataEnd + 1 == sendDataStart
            || (sendDataEnd + 1 == sendBufferEnd && sendDataStart == sendBufferStart);
    }

    void PacketCommunicationSLIPBuffered::SendDone(uint8_t* end)
    {
        sending = false;

        sendDataStart = end;
        if (sendDataStart == sendBufferEnd)
            sendDataStart = sendBufferStart;

        TryStartSending();
    }

    void PacketCommunicationSLIPBuffered::TryStartSending()
    {
        if (!sending && sendDataStart != sendDataEnd)
        {
            sending = true;
            StartSending(sendDataStart, sendDataStart < sendDataEnd ? sendDataEnd : sendBufferEnd);
        }
    }
}