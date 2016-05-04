#include "PCString.hpp"
#include <string.h>

namespace erpc
{
    PCString::PCString()
    {
        text[0] = 0;
    }

    PCString::PCString(const char* str)
    {
        strncpy(text, str, sizeof(text));
        text[sizeof(text)-1] = 0;
    }


    void PCString::Write(PacketCommunication& packetComm) const
    {
        text[PCSTRING_LEN - 1] = 0;
        packetComm.Write((uint8_t*)text, strlen(text) + 1);
    }

    bool PCString::Read(PacketCommunication& packetComm)
    {
        uint8_t i = 0;
        uint8_t c;

        while (packetComm.Read(c))
        {
            text[i++] = (char)c;
            if (c == 0) return true;
            if (i == PCSTRING_LEN) return false;
        }
        return false;
    }
}