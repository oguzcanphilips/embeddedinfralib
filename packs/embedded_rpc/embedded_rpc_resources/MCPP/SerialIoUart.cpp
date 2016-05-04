#include "SerialIoUart.h"

SerialIoUart::SerialIoUart(String^ port, uint32_t baudrate)
    : mSerialIO(gcnew SerialPort())
    , mSendBuffer(gcnew array<uint8_t>(1))
{
    mSerialIO->PortName = port;
    mSerialIO->BaudRate = baudrate;
    mSerialIO->Parity = Parity::None;
    mSerialIO->DataBits = 8;
    mSerialIO->StopBits = StopBits::One;
    mSerialIO->Handshake = Handshake::None;
    mSerialIO->DtrEnable = false;
    mSerialIO->ReceivedBytesThreshold = 1;
    mSerialIO->WriteTimeout = 1000;
    mSerialIO->ReadTimeout = 1000;
    
    mSerialIO->Open();
    if (!mSerialIO->IsOpen)
        throw gcnew Exception("Could not open com port : " + port);
}

SerialIoUart::~SerialIoUart()
{
    mSerialIO->Close();
}
    
void SerialIoUart::Open()
{
    mSerialIO->Open();
}

bool SerialIoUart::IsOpen()
{
    return mSerialIO->IsOpen;
}

void SerialIoUart::Close()
{
    mSerialIO->Close();
}

void SerialIoUart::Send(uint8_t v)
{
    mSendBuffer[0] = v;
    try
    {
        if(!mSerialIO->IsOpen)
        {
            mSerialIO->Open();
        }
        mSerialIO->Write(mSendBuffer, 0, 1);
    }
    catch(...)
    {
        try
        {
            mSerialIO->Close();
        }
        catch(...){}
    }  
}

bool SerialIoUart::Receive(uint8_t% v)
{
    try
    {
        int32_t ret = mSerialIO->ReadByte();
        v = (uint8_t) ret;
        return (ret >= 0);
    }
    catch(Exception^)
    {
        return false;
    }
    return false;
}
