#ifndef LOCALPACKETCOMMUNICATIONASCII_HPP
#define LOCALPACKETCOMMUNICATIONASCII_HPP

#include "PacketCommunicationAscii.hpp"
#include <list>

class LocalPacketCommunicationAscii : public erpc::PacketCommunicationAscii
{
    std::list<char> fifo;
    LocalPacketCommunicationAscii* mLink;
public:
    LocalPacketCommunicationAscii()
        : mLink(0)
    {
    }
    void Link(LocalPacketCommunicationAscii* comm)
    {
        comm->mLink = this;
        mLink = comm;
    }

    void ProcessReceive()
    {
        if (fifo.empty()) return;
        Receive();
    }
#include <iostream>
    void WriteByte(uint8_t data)
    {
        std::cout << (char)data;
        mLink->fifo.push_back(data);
    }

    bool ReadByte(uint8_t& v)
    {
        if (fifo.empty())
            return false;
        v = fifo.front();
        fifo.pop_front();
        return true;
    }

    void PacketDone() override
    {
        PacketCommunicationAscii::PacketDone();
        switch (mode)
        {
        case OnEndSend:
            mLink->ProcessReceive();
            break;
        case OnEndSendReceive:
            mLink->ProcessReceive();
            ProcessReceive();
            break;
        default:
            break;
        }
    }

    enum Mode
    {
        OnEndNone,
        OnEndSend,
        OnEndSendReceive
    };

    void SetMode(Mode m)
    {
        mode = m;
    }
private:
    Mode mode = OnEndNone;
};

#endif