#include "PCString.hpp"
#include <string>

using namespace System::Runtime::InteropServices;
namespace erpc
{
    PCString::PCString()
    {
        text = "";
    }

    void PCString::Write(PacketCommunication^ packetComm)
    {
        char* str = (char*)(void*)Marshal::StringToHGlobalAnsi(text);
        std::string text = str;
        Marshal::FreeHGlobal((System::IntPtr)str);
        packetComm->Write(reinterpret_cast<const uint8_t*>(text.c_str()), text.length()+1);
    }

    bool PCString::Read(PacketCommunication^ packetComm)
    {
        char input[PCSTRING_LEN];
        uint8_t i = 0;
        uint8_t c;

        while (packetComm->Read(c))
        {
            input[i++] = (char)c;
            if (c == 0)
            {
                text = gcnew String(input);
                return true;
            }
            if (i == PCSTRING_LEN) return false;
        }
        return false;
    }
}