#ifndef PCSTRING_H
#define PCSTRING_H

#include "PacketCommunication.h"

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