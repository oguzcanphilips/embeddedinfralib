#include "PacketCommunication.hpp"
#include <cstdlib>

namespace erpc
{
    PacketCommunication::Callback* PacketCommunication::Callbacks = 0;
    uint32_t PacketCommunication::UsedIdMasks = 0;

    PacketCommunication::PacketCommunication()
        : mIdMask(ObtainIdMask())
    {
    }

    PacketCommunication::~PacketCommunication()
    {
        ReleaseIdMask(mIdMask);
    }

    uint8_t PacketCommunication::WriteMessageId()
    {
        uint8_t id = ++messageId;
        WriteMessageId(id);
        return id;
    }

    uint32_t PacketCommunication::ObtainIdMask()
    {
        uint32_t mask = 1;
        while (mask)
        {
            if (!(mask & UsedIdMasks))
            {
                UsedIdMasks |= mask;
                return mask;
            }
            mask <<= 1;
        }
        std::abort();
    }

    void PacketCommunication::ReleaseIdMask(uint32_t idMask)
    {
        for (Callback* it = Callbacks; it; it = it->mNext)
            it->registerMask &= ~idMask;

        UsedIdMasks &= ~idMask;
    }

    void PacketCommunication::Register(Callback& callback)
    {
        callback.registerMask |= mIdMask;
        for (Callback* it = Callbacks; it; it = it->mNext)
        {
            if (it == &callback) return;
        }
        callback.mNext = Callbacks;
        Callbacks = &callback;
    }

    void PacketCommunication::Unregister(Callback& callback)
    {
        callback.registerMask &= ~mIdMask;
        if (callback.registerMask) return;

        Remove(&callback);
    }

    void PacketCommunication::Remove(Callback* callback)
    {
        if (Callbacks == 0) return;

        if (Callbacks == callback)
        {
            Callbacks = Callbacks->mNext;
            return;
        }

        Callback* prevIt = Callbacks;
        for (Callback* it = Callbacks->mNext; it; it = it->mNext)
        {
            if (it == callback)
            {
                prevIt->mNext = it->mNext;
                return;
            }
            prevIt = it;
        }
    }

    void PacketCommunication::HandleReceiveError()
    {
    }

    void PacketCommunication::Receive()
    {
        uint8_t interfaceId;
        if (!ReadStartToken(interfaceId))
            return;

        for (Callback* it = Callbacks; it; it = it->mNext)
        {
            if ((it->mInterfaceId &0x7f) == (interfaceId & 0x7f) && 
                it->registerMask & mIdMask)
            {
                it->Receive(*this);
                return;
            }
        }
        ForwardReceive(interfaceId);
    }
 
    void PacketCommunication::ForwardReceive(uint8_t interfaceId)
    {
    }
}