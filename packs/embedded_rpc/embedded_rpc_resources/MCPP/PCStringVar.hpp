#ifndef PCSTRINGVAR_H
#define PCSTRINGVAR_H

#include "PacketCommunication.h"

using namespace System;

public ref class PCStringVar : public Serialize
{
public:
    String^ text;

    PCStringVar();
    virtual void Write(PacketCommunication^ packetComm);
    virtual bool Read(PacketCommunication^ packetComm);
};

#endif