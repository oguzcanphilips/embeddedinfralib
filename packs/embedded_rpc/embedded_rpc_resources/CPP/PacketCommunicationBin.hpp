#ifndef PACKETCOMMUNICATIONBIN_HPP
#define PACKETCOMMUNICATIONBIN_HPP

#include "PacketCommunication.hpp"

namespace erpc
{
    class PacketCommunicationBin : public PacketCommunication
    {
    public:
        PacketCommunicationBin();
        void Link(PacketCommunicationBin& link);

        void PacketStart(uint8_t interfaceId, uint8_t functionId) override;
        void PacketDone() override;
        bool ReadDone() override;

        void Write(uint8_t v) override;
        void Write(uint16_t v) override;
        void Write(uint32_t v) override;
        void Write(int8_t v) override;
        void Write(int16_t v) override;
        void Write(int32_t v) override;
        void Write(bool v) override;
        void Write(const Serialize& obj) override;
        void Write(const uint8_t* data, uint16_t len) override;

        bool Read(uint8_t& v) override;
        bool Read(uint16_t& v) override;
        bool Read(uint32_t& v) override;
        bool Read(int8_t& v) override;
        bool Read(int16_t& v) override;
        bool Read(int32_t& v) override;
        bool Read(bool& v) override;
        bool Read(Serialize& obj) override;
        bool Read(uint8_t* data, uint16_t len) override;
        
    protected:
        virtual void PacketStartToken() = 0;
        virtual void PackedEndToken() = 0;

        bool ReadStartToken(uint8_t& interfaceId) override;
        void ForwardReceive(uint8_t interfaceId);

    private:
        void WriteInternal(uint8_t v);
        bool ReadInternal(uint8_t& v);

        PacketCommunicationBin* mLink = nullptr;

#ifdef VALIDATION_CRC
        uint16_t mCrcWr = 0;
        uint16_t mCrcRd = 0;
#endif
#ifdef VALIDATION_CHECKSUM
        uint8_t mChecksumWr = 0;
        uint8_t mChecksumRd = 0;
#endif
    };
}
#endif
