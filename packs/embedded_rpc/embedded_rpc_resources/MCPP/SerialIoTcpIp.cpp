#include "SerialIoTcpIp.h"

SerialIoTcpIp::SerialIoTcpIp(String^ hostname, uint32_t port)
    : mTcpClient(nullptr)
    , mNetworkStream(nullptr)
    , mSendBuffer(gcnew array<uint8_t>(1))
    , mHostname(hostname)
    , mPort(port)
{
}

SerialIoTcpIp::~SerialIoTcpIp()
{
    Close();
}

void SerialIoTcpIp::Open()
{
    Close();
    
    mTcpClient = gcnew TcpClient();
    mTcpClient->SendTimeout = 30000;
    mTcpClient->Connect(mHostname, mPort);

    if (!mTcpClient->Connected)
    {
        Console::WriteLine("Failed opening TCP connection to {0}:{1}... ", mHostname, mPort);
        throw gcnew Exception("Could not open TCP connection to : " + mHostname + " port " + mPort);
    }
    else
    {
        mNetworkStream = mTcpClient->GetStream();
        Console::WriteLine("Successfully opened TCP connection to {0}:{1}... ", mHostname, mPort);
    }
}

bool SerialIoTcpIp::IsOpen()
{
    return mTcpClient != nullptr && mTcpClient->Connected;
}

void SerialIoTcpIp::Close()
{
    if (mTcpClient != nullptr)
    {
        Console::WriteLine("Closing TCP connection to {0}:{1}", mHostname, mPort);

        mNetworkStream->Close();
        mNetworkStream = nullptr;
        mTcpClient->Close();
        mTcpClient = nullptr;
    }
}

void SerialIoTcpIp::Send(uint8_t v)
{
    mSendBuffer[0] = v;
    try
    {
        mNetworkStream->Write(mSendBuffer, 0, 1);
    }
    catch (Exception^ e)
    {
        Console::WriteLine("!! Error writing byte '{0}'; exception: {1}", v, e->ToString());
        throw;
    }
}

bool SerialIoTcpIp::Receive(uint8_t% v)
{
    try
    {
        if (CanRead())
        {
            int32_t ret = mNetworkStream->ReadByte();
            v = (uint8_t)ret;
            return (ret >= 0);
        }
        else
        {
            return false;
        }
    }
    catch (Exception^ e)
    {
        Console::WriteLine("!! Error reading byte; exception: {0}", e->ToString());
        return false;
    }
    return false;
}

bool SerialIoTcpIp::CanRead()
{
    return (mTcpClient != nullptr) && mTcpClient->Connected && (mNetworkStream != nullptr);
}
