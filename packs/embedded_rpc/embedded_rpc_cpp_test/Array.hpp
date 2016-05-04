#ifndef ARRAY_H
#define ARRAY_H

#include "PacketCommunication.hpp"

namespace erpc
{
    class Array : public Serialize
    {
        uint8_t mData[10];
    public:
        Array()
        {
            for (uint32_t i = 0; i < sizeof(mData); ++i) mData[i] = 0;
        }
        uint8_t& operator[](uint32_t index)
        {
            return mData[index];
        }

        void Write(PacketCommunication& packetComm) const
        {
            packetComm.Write(mData, sizeof(mData));
        }

        bool Read(PacketCommunication& packetComm)
        {
            return packetComm.Read(mData, sizeof(mData));
        }
    };
}

#endif