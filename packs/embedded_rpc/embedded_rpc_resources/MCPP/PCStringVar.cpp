#include "PCStringVar.h"
#include <string>

using namespace System::Runtime::InteropServices;

PCStringVar::PCStringVar()
{
    text = "";
}

void PCStringVar::Write(PacketCommunication^ packetComm)
{
    char* str = (char*)(void*)Marshal::StringToHGlobalAnsi(text);
    std::string text = str;
    Marshal::FreeHGlobal((System::IntPtr)str); 

    uint8_t* data = (uint8_t*)text.c_str();
    while(*data)
    {
        packetComm->Write(*data);
        data++;
    }
	
    packetComm->Write((uint8_t)0);
}

bool PCStringVar::Read(PacketCommunication^ packetComm)
{
    text = gcnew String("");

    uint8_t c;

    while(packetComm->Read(c))
    {
        if(c==0)
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
