#include "PacketCommunicationBin.hpp"
#include <cstdlib>

#ifdef VALIDATION_CRC
#include "../CRC.h"
#endif

namespace erpc
{
    PacketCommunicationBin::PacketCommunicationBin()
        : PacketCommunication()
#ifdef VALIDATION_CRC
        , mCrcWr(0)
        , mCrcRd(0)
#endif
#ifdef VALIDATION_CHECKSUM
        , mChecksumWr(0)
        , mChecksumRd(0)
#endif
    {
    }
    void PacketCommunicationBin::Write(uint8_t v)
    {
        WriteInternal(v);
    }

    void PacketCommunicationBin::Write(uint16_t v)
    {
        WriteInternal((uint8_t)(v >> 8));
        WriteInternal((uint8_t)(v & 0xff));
    }

    void PacketCommunicationBin::Write(uint32_t v)
    {
        WriteInternal((uint8_t)(v >> 24));
        WriteInternal((uint8_t)((v >> 16) & 0xff));
        WriteInternal((uint8_t)((v >> 8) & 0xff));
        WriteInternal((uint8_t)(v & 0xff));
    }

    void PacketCommunicationBin::Write(const uint8_t* data, uint16_t len)
    {
        while (len)
        {
            WriteInternal(*data);
            data++;
            len--;
        }
    }

    void PacketCommunicationBin::Write(int8_t v)
    {
        Write((uint8_t)v);
    }

    void PacketCommunicationBin::Write(int16_t v)
    {
        Write((uint16_t)v);
    }

    void PacketCommunicationBin::Write(int32_t v)
    {
        Write((uint32_t)v);
    }

    void PacketCommunicationBin::Write(bool v)
    {
        Write((uint8_t)(v ? 1 : 0));
    }

    void PacketCommunicationBin::Write(const Serialize& obj)
    {
        obj.Write(*this);
    }


    bool PacketCommunicationBin::Read(uint8_t& v)
    {
        return ReadInternal(v);
    }

    bool PacketCommunicationBin::Read(uint16_t& v)
    {
        uint8_t c;
        if (!Read(c)) return false;
        v = c;
        v <<= 8;
        if (!Read(c)) return false;
        v |= c;
        return true;
    }

    bool PacketCommunicationBin::Read(uint32_t& v)
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

    bool PacketCommunicationBin::Read(bool& v)
    {
        uint8_t c;
        if (!Read(c)) return false;
        v = c ? true : false;
        return true;
    }

    bool PacketCommunicationBin::Read(uint8_t* data, uint16_t len)
    {
        for (uint16_t i = 0; i < len; ++i)
        {
            if (!Read(data[i])) return false;
        }
        return true;
    }

    bool PacketCommunicationBin::Read(int8_t& v)
    {
        return Read((uint8_t&)v);
    }

    bool PacketCommunicationBin::Read(int16_t& v)
    {
        return Read((uint16_t&)v);
    }

    bool PacketCommunicationBin::Read(int32_t& v)
    {
        return Read((uint32_t&)v);
    }

    bool PacketCommunicationBin::Read(Serialize& obj)
    {
        return obj.Read(*this);
    }

    void PacketCommunicationBin::WriteInternal(uint8_t v)
    {
#ifdef VALIDATION_CRC
        CRC_Update(&mCrcWr, v);
#endif
#ifdef VALIDATION_CHECKSUM
        mChecksumWr += v;
#endif
        WriteByte(v);
    }

    bool PacketCommunicationBin::ReadInternal(uint8_t& v)
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

    void PacketCommunicationBin::PacketStart(uint8_t interfaceId, uint8_t functionId)
    {
#ifdef VALIDATION_CRC
        mCrcWr = 0;
#endif
#ifdef VALIDATION_CHECKSUM
        mChecksumWr = 0;
#endif
        PacketStartToken();
        WriteInternal(interfaceId);
        WriteInternal(functionId);
    }

    void PacketCommunicationBin::PacketDone()
    {
#ifdef VALIDATION_CRC
        WriteByte(mCrcWr >> 8);
        WriteByte(mCrcWr & 0xff);
#endif
#ifdef VALIDATION_CHECKSUM
        WriteByte(mChecksumWr);
#endif
        PackedEndToken();
    }

    bool PacketCommunicationBin::ReadDone()
    {
#if defined(VALIDATION_CRC)
        uint8_t dummy;
        if (!ReadInternal(dummy) || !ReadInternal(dummy)) return false;
        return IsPacketEnded() && (mCrcRd == 0);
#elif defined(VALIDATION_CHECKSUM)
        uint8_t checksum;
        if (!ReadByte(checksum)) return false;
        return IsPacketEnded() && (mChecksumRd == checksum);
#else
        return IsPacketEnded();
#endif
    }

    bool PacketCommunicationBin::ReadInterfaceId(uint8_t& id)
    {
#ifdef VALIDATION_CRC
        mCrcRd = 0;
#endif
#ifdef VALIDATION_CHECKSUM
        mChecksumRd = 0;
#endif
        return Read(id);
    }
}