#ifndef PCSTRINGVAR_HPP
#define PCSTRINGVAR_HPP

#include "PacketCommunication.hpp"
#include <string>

namespace erpc
{
    class PCStringVar
        : public Serialize
    {
    public:
        std::string text;

        PCStringVar();
        PCStringVar(const std::string& str);
        void Write(PacketCommunication& packetComm) const;
        bool Read(PacketCommunication& packetComm);
    };
}
#endif