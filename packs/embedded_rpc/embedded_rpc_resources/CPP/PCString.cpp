#include "PCString.hpp"
#include <string.h>

namespace erpc
{
    PCString::PCString()
    {
        text[0] = 0;
    }

    PCString::~PCString()
    {
#ifdef PCSTRING_ALLOW_HEAP
        delete[] pText;
#endif
    }

    PCString::PCString(const PCString& string)
    {
        Assign(string);
    }

    const PCString& PCString::operator=(const PCString& string)
    {
        if (this != &string)
            Assign(string);

        return *this;
    }

    PCString::PCString(const char* str)
    {
        Assign(str);
    }

    void PCString::operator=(const char* str)
    {
        Assign(str);
    }

    bool PCString::operator==(const PCString& str) const
    {
        const char* a = *this;
        const char* b = str;
        while (*a == *b)
        {
            if (*a == 0)
                return true;
            a++;
            b++;
        }
        return false;
    }

    PCString::operator const char*() const
    {
#ifdef PCSTRING_ALLOW_HEAP
        return reinterpret_cast<const char*>(pText ? pText : text.data());
#else
        return reinterpret_cast<const char*>(text.data());
#endif
    }

    void PCString::Assign(const char* str)
    {
#ifdef PCSTRING_ALLOW_HEAP
        delete pText;
        pText = nullptr;
        uint32_t len = strlen(str);
        if (len >= PCSTRING_LEN)
        {
            pText = new uint8_t[len + 1];
            memcpy(pText, str, len + 1);
        }
        else
            memcpy(text.data(), str, len + 1);
#else
        uint32_t len = strlen(str);
        if (len >= PCSTRING_LEN)
            len = PCSTRING_LEN - 1;
        memcpy(text.data(), str, len);
        text.data()[len] = 0;
#endif
    }


    void PCString::Write(PacketCommunication& packetComm) const
    {
        const char* str = *this;
        packetComm.Write(str);
    }

    bool PCString::Read(PacketCommunication& packetComm)
    {
        uint8_t i = 0;
        uint8_t c;

        uint8_t* dest = reinterpret_cast<uint8_t*>(text.data());
        while (packetComm.Read(c))
        {
            dest[i++] = (char)c;
            if (c == 0) return true;
            if ((i % PCSTRING_LEN) == 0)
#ifdef PCSTRING_ALLOW_HEAP
            {
                uint8_t* newDest = new uint8_t[i + PCSTRING_LEN];
                memcpy(newDest, dest, i);
                delete [] pText;
                dest = pText = newDest;

            }
#else
            {
                return false;
            }
#endif
        }
        return false;
    }
}