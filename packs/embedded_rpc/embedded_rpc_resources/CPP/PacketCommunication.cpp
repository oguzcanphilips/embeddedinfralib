#include "PacketCommunication.hpp"
#include <cstdlib>

namespace erpc
{
#ifdef VALIDATION_CRC
#include "../CRC.h"
#endif
    PacketCommunication::Callback* PacketCommunication::Callbacks = 0;
    uint32_t PacketCommunication::UsedIdMasks = 0;

    PacketCommunication::PacketCommunication()
        : mIdMask(ObtainIdMask())
#ifdef VALIDATION_CRC
        , mCrcWr(0)
        , mCrcRd(0)
#endif
#ifdef VALIDATION_CHECKSUM
        , mChecksumWr(0)
        , mChecksumRd(0)
#endif
        , mLink(0)
    {
    }

    PacketCommunication::~PacketCommunication()
    {
        ReleaseIdMask(mIdMask);
    }

    void PacketCommunication::Link(PacketCommunication& link)
    {
        mLink = &link;
        link.mLink = this;
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
#ifdef VALIDATION_CRC
        mCrcRd = 0;
#endif
#ifdef VALIDATION_CHECKSUM
        mChecksumRd = 0;
#endif
        if (!Read(interfaceId)) return;
        for (Callback* it = Callbacks; it; it = it->mNext)
        {
            if (it->mInterfaceId == interfaceId)
            {
                if (it->registerMask & mIdMask)
                {
                    it->Receive(*this);
                    return;
                }
            }
        }
        // not handled, forward to link
        if (mLink)
        {
            mLink->PacketStart();
            mLink->WriteByte(interfaceId);
            uint8_t data;
            while (Read(data))
            {
                mLink->WriteByte(data);
            }
            mLink->PacketEnd();
        }
    }

    void PacketCommunication::PacketStart(uint8_t interfaceId, uint8_t functionId)
    {
#ifdef VALIDATION_CRC
        mCrcWr = 0;
#endif
#ifdef VALIDATION_CHECKSUM
        mChecksumWr = 0;
#endif
        PacketStart();
        WriteInternal(interfaceId);
        WriteInternal(functionId);
    }

    void PacketCommunication::PacketDone()
    {
#ifdef VALIDATION_CRC
        WriteByte(mCrcWr >> 8);
        WriteByte(mCrcWr & 0xff);
#endif
#ifdef VALIDATION_CHECKSUM
        WriteByte(mChecksumWr);
#endif
        PacketEnd();
    }

    void PacketCommunication::Write(uint8_t v)
    {
        WriteInternal(v);
    }

    void PacketCommunication::Write(uint16_t v)
    {
        WriteInternal((uint8_t)(v >> 8));
        WriteInternal((uint8_t)(v & 0xff));
    }

    void PacketCommunication::Write(uint32_t v)
    {
        WriteInternal((uint8_t)(v >> 24));
        WriteInternal((uint8_t)((v >> 16) & 0xff));
        WriteInternal((uint8_t)((v >> 8) & 0xff));
        WriteInternal((uint8_t)(v & 0xff));
    }

    void PacketCommunication::Write(const uint8_t* data, uint16_t len)
    {
        while (len)
        {
            WriteInternal(*data);
            data++;
            len--;
        }
    }

    void PacketCommunication::Write(int8_t v)
    {
        Write((uint8_t)v);
    }

    void PacketCommunication::Write(int16_t v)
    {
        Write((uint16_t)v);
    }

    void PacketCommunication::Write(int32_t v)
    {
        Write((uint32_t)v);
    }

    void PacketCommunication::Write(bool v)
    {
        Write((uint8_t)(v ? 1 : 0));
    }

    void PacketCommunication::Write(const Serialize& obj)
    {
        obj.Write(*this);
    }


    bool PacketCommunication::Read(uint8_t& v)
    {
        return ReadInternal(v);
    }

    bool PacketCommunication::Read(uint16_t& v)
    {
        uint8_t c;
        if (!Read(c)) return false;
        v = c;
        v <<= 8;
        if (!Read(c)) return false;
        v |= c;
        return true;
    }

    bool PacketCommunication::Read(uint32_t& v)
    {
        uint8_t c;
        if (!Read(c)) return false;
        v = c;
        v <<= 8;
        if (!Read(c)) return false;
        v |= c;
        v <<= 8;
        if (!Read(c)) return false;
        v |= c;
        v <<= 8;
        if (!Read(c)) return false;
        v |= c;
        return true;
    }

    bool PacketCommunication::Read(bool& v)
    {
        uint8_t c;
        if (!Read(c)) return false;
        v = c ? true : false;
        return true;
    }

    bool PacketCommunication::Read(uint8_t* data, uint16_t len)
    {
        for (uint16_t i = 0; i < len; ++i)
        {
            if (!Read(data[i])) return false;
        }
        return true;
    }

    bool PacketCommunication::Read(int8_t& v)
    {
        return Read((uint8_t&)v);
    }

    bool PacketCommunication::Read(int16_t& v)
    {
        return Read((uint16_t&)v);
    }

    bool PacketCommunication::Read(int32_t& v)
    {
        return Read((uint32_t&)v);
    }

    bool PacketCommunication::Read(Serialize& obj)
    {
        return obj.Read(*this);
    }

    void PacketCommunication::WriteInternal(uint8_t v)
    {
#ifdef VALIDATION_CRC
        CRC_Update(&mCrcWr, v);
#endif
#ifdef VALIDATION_CHECKSUM
        mChecksumWr += v;
#endif
        WriteByte(v);
    }

    bool PacketCommunication::ReadInternal(uint8_t& v)
    {
        if (!ReadByte(v))
        {
            return false;
        }
#ifdef VALIDATION_CRC
        CRC_Update(&mCrcRd, v);
#endif
#ifdef VALIDATION_CHECKSUM
        mChecksumRd += v;
#endif
        return true;
    }

    bool PacketCommunication::ReadDone()
    {
#if defined(VALIDATION_CRC)
        uint8_t dummy;
        if(!ReadInternal(dummy) || !ReadInternal(dummy)) return false;
        return IsPacketEnded() && (mCrcRd == 0);
#elif defined(VALIDATION_CHECKSUM)
        uint8_t checksum;
        if(!ReadByte(checksum)) return false;
        return IsPacketEnded() && (mChecksumRd == checksum);
#else
        return IsPacketEnded();
#endif
    }
}