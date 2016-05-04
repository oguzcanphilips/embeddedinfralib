#ifndef PACKETCOMMUNICATIONSerialIoUART_H
#define PACKETCOMMUNICATIONSerialIoUART_H

#include "PacketCommunicationSLIP.h"
using namespace System;
using namespace System::IO::Ports;

public ref class SerialIoUart : public PacketCommunicationSLIP::ISerialIO
{
public:
    SerialIoUart(String^ port, uint32_t baudrate);
    ~SerialIoUart();

    virtual void Open();
    virtual bool IsOpen();
    virtual void Close();
    virtual void Send(uint8_t v);
    virtual bool Receive(uint8_t% v);
private:
    SerialPort^ mSerialIO;
    array<uint8_t>^ mSendBuffer;
};

#endif