#include "PacketCommunicationSLIP.hpp"
#include "../PacketCommunicationDefines.h"

namespace erpc
{
    PacketCommunicationSLIP::PacketCommunicationSLIP(ISerialIO^ serialIO)
        : mSerialIO(serialIO)
    {
        Start();
    }

    PacketCommunicationSLIP::~PacketCommunicationSLIP()
    {
        Stop();
    }

    void PacketCommunicationSLIP::Stop()
    {
        mSerialIO->Close();
        PacketCommunication::Stop();
    }

    void PacketCommunicationSLIP::ProcessReceive()
    {
        uint8_t v;
        do
        {
            v = ReadInternal();
        } while (v != SLIP_BEGIN && v != SLIP_END);
        if (v == SLIP_BEGIN) Receive();
    }

    bool PacketCommunicationSLIP::IsPacketEnded()
    {
        return ReadInternal() == SLIP_END;
    }

    void PacketCommunicationSLIP::PacketStartToken()
    {
        Send(SLIP_BEGIN);
    }

    void PacketCommunicationSLIP::PackedEndToken()
    {
        Send(SLIP_END);
    }

    void PacketCommunicationSLIP::WriteByte(uint8_t v)
    {
        switch (v)
        {
        case SLIP_BEGIN: Send(SLIP_ESC); Send(SLIP_ESC_BEGIN); break;
        case SLIP_END:   Send(SLIP_ESC); Send(SLIP_ESC_END);   break;
        case SLIP_ESC:   Send(SLIP_ESC); Send(SLIP_ESC_ESC);   break;
        default:    Send(v); break;
        }
    }

    bool PacketCommunicationSLIP::ReadByte(uint8_t% v)
    {
        v = ReadInternal();
        if (SLIP_END == v || SLIP_BEGIN == v)
        {
            return false;
        }
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

    void PacketCommunicationSLIP::Send(uint8_t v)
    {
        try
        {
            if (!mSerialIO->IsOpen())
            {
                mSerialIO->Open();
            }
            mSerialIO->Send(v);
        }
        catch (...)
        {
            try
            {
                mSerialIO->Close();
            }
            catch (...){}
            throw;
        }
    }

    uint8_t PacketCommunicationSLIP::ReadInternal()
    {
        try
        {
            uint8_t v;
            if (!mSerialIO->IsOpen() || !mSerialIO->Receive(v))
            {
                Thread::Sleep(100);
                return SLIP_END;
            }

            return v;
        }
        catch (Exception^)
        {
        }
        return SLIP_END;
    }
}