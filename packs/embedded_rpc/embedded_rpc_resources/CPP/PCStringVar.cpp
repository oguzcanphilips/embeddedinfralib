#include "PCStringVar.hpp"
#include <string.h>

namespace erpc
{
    PCStringVar::PCStringVar()
    {
    }

    PCStringVar::PCStringVar(const std::string& str)
        :text(str)
    {
    }

    void PCStringVar::Write(PacketCommunication& packetComm) const
    {
        packetComm.Write(reinterpret_cast<const uint8_t*>(text.c_str()), text.length() + 1);
    }

    bool PCStringVar::Read(PacketCommunication& packetComm)
    {
        text = "";

        uint8_t c;

        while (packetComm.Read(c))
        {
            if (c == 0)
                return true;

            text += (char)c;
        }
        return false;
    }
}