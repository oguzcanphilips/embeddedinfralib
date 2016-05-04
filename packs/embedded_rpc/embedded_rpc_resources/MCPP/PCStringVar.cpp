#include "PCStringVar.hpp"
#include <string>

using namespace System::Runtime::InteropServices;
namespace erpc
{
    PCStringVar::PCStringVar()
    {
        text = "";
    }

    void PCStringVar::Write(PacketCommunication^ packetComm)
    {
        char* str = (char*)(void*)Marshal::StringToHGlobalAnsi(text);
        std::string text = str;
        Marshal::FreeHGlobal((System::IntPtr)str);
        packetComm->Write(reinterpret_cast<const uint8_t*>(text.c_str()), text.length() + 1);
    }

    bool PCStringVar::Read(PacketCommunication^ packetComm)
    {
        text = gcnew String("");

        uint8_t c;

        while (packetComm->Read(c))
        {
            if (c == 0)
            {
                return true;
            }
            else
            {
                char input = (char)c;
                text += gcnew String(&input);
            }
        }
        return false;
    }
}