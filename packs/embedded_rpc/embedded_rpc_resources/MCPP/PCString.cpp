#include "PCString.hpp"
#include <string>

using namespace System::Runtime::InteropServices;

PCString::PCString()
{
    text = "";
}

void PCString::Write(PacketCommunication^ packetComm)
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

bool PCString::Read(PacketCommunication^ packetComm)
{
    char input[PCSTRING_LEN];
    uint8_t i = 0;
    uint8_t c;

    while(packetComm->Read(c))
    {
        input[i++] = (char)c;
        if(c==0)
        {
            text = gcnew String(input);
            return true;
        }
        if(i==PCSTRING_LEN) return false;
    }
    return false;
}
