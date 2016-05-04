#ifndef PCSTRING_HPP
#define PCSTRING_HPP

#include "PacketCommunication.hpp"

using namespace System;

public ref class PCString : public Serialize
{
public:
    String^ text;

    PCString();
    virtual void Write(PacketCommunication^ packetComm);
    virtual bool Read(PacketCommunication^ packetComm);
};

#endif