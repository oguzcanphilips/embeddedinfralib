#ifndef PCSTRINGVAR_HPP
#define PCSTRINGVAR_HPP

#include "PacketCommunication.hpp"

using namespace System;
namespace erpc
{
    public ref class PCStringVar : public Serialize
    {
    public:
        String^ text;

        PCStringVar();
        virtual void Write(PacketCommunication^ packetComm);
        virtual bool Read(PacketCommunication^ packetComm);
    };
}
#endif