#include "PacketCommunicationSLIP.hpp"
#include "../PacketCommunicationDefines.h"

namespace erpc
{
    PacketCommunicationSLIP::PacketCommunicationSLIP(ISerialIO& serialIO)
        : mSerialIO(serialIO)
    {}

    void PacketCommunicationSLIP::ProcessReceive()
    {
        uint8_t v;
        do
        {
            v = ReadInternal();
        } while (v != SLIP_BEGIN && v != SLIP_END);

        if (v == SLIP_BEGIN)
            Receive();
    }

    bool PacketCommunicationSLIP::IsPacketEnded()
    {
        return ReadInternal() == SLIP_END;
    }

    void PacketCommunicationSLIP::PacketEnd()
    {
        mSerialIO.Send(SLIP_END);
    }

    void PacketCommunicationSLIP::WriteByte(uint8_t data)
    {
        switch (data)
        {
        case SLIP_BEGIN: mSerialIO.Send(SLIP_ESC); mSerialIO.Send(SLIP_ESC_BEGIN); break;
        case SLIP_END:   mSerialIO.Send(SLIP_ESC); mSerialIO.Send(SLIP_ESC_END);   break;
        case SLIP_ESC:   mSerialIO.Send(SLIP_ESC); mSerialIO.Send(SLIP_ESC_ESC);   break;
        default:    mSerialIO.Send(data);  break;
        }
    }

    bool PacketCommunicationSLIP::ReadByte(uint8_t& v)
    {
        v = ReadInternal();
        if (SLIP_END == v || SLIP_BEGIN == v)
            return false;

        if (SLIP_ESC == v)
        {
            switch (ReadInternal())
            {
            case SLIP_ESC_BEGIN: v = SLIP_BEGIN; break;
            case SLIP_ESC_END:   v = SLIP_END;   break;
            case SLIP_ESC_ESC:   v = SLIP_ESC;   break;
            default:
                return false;
            }
        }

        return true;
    }

    uint8_t PacketCommunicationSLIP::ReadInternal()
    {
        uint8_t r = SLIP_END;
        if (mSerialIO.Receive(r))
            return r;
        else
            return SLIP_END;
    }

    void PacketCommunicationSLIP::PacketStart()
    {
        mSerialIO.Send(SLIP_BEGIN);
    }
}