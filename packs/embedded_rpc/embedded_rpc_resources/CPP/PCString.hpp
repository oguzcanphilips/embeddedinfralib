#ifndef PCSTRING_HPP
#define PCSTRING_HPP

#include "PacketCommunication.hpp"

namespace erpc
{
    class PCString : public Serialize
    {
    public:
        mutable char text[PCSTRING_LEN];

        PCString();
        PCString(const char* str);
        void Write(PacketCommunication& packetComm) const;
        bool Read(PacketCommunication& packetComm);
    };
}
#endif