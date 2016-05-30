#ifndef PCSTRING_HPP
#define PCSTRING_HPP

#include "PacketCommunication.hpp"
#include <array>

namespace erpc
{
    class PCString : public Serialize
    {
    public:
        PCString();
        ~PCString();
        PCString(const PCString& string);
        const PCString& operator=(const PCString& string);
        PCString(const char* str);

        void operator=(const char* text);
        operator const char*() const;

        bool operator==(const PCString& str) const;

        void Write(PacketCommunication& packetComm) const;
        bool Read(PacketCommunication& packetComm);


    private:
        void Assign(const char* text);
        std::array<uint8_t, PCSTRING_LEN> text;
#ifdef PCSTRING_ALLOW_HEAP
        uint8_t* pText = nullptr;
#endif
    };
}
#endif