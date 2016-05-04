#ifndef PACKETCOMMUNICATIONTCPIP_H
#define PACKETCOMMUNICATIONTCPIP_H

#include "PacketCommunicationSLIP.h"
using namespace System;
using namespace System::Net::Sockets;

public ref class SerialIoTcpIp : public PacketCommunicationSLIP::ISerialIO
{
public:
    SerialIoTcpIp(String^ hostname, uint32_t port);
    ~SerialIoTcpIp();

    virtual void Open();
    virtual bool IsOpen();
    virtual void Close();
    virtual void Send(uint8_t v);
    virtual bool Receive(uint8_t% v);

private:
    bool CanRead();

    TcpClient^ mTcpClient;
    NetworkStream^ mNetworkStream;
    array<uint8_t>^ mSendBuffer;
    String^ mHostname;
    uint32_t mPort;
};

#endif
