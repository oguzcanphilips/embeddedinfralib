#ifndef PACKETCOMMUNICATIONASCII_HPP
#define PACKETCOMMUNICATIONASCII_HPP

#include "PacketCommunication.hpp"

namespace erpc
{
    class PacketCommunicationAscii : public PacketCommunication
    {
    public:
        PacketCommunicationAscii();

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
        void Write(const char* string) override;

        bool Read(uint8_t& v) override;
        bool Read(uint16_t& v) override;
        bool Read(uint32_t& v) override;
        bool Read(int8_t& v) override;
        bool Read(int16_t& v) override;
        bool Read(int32_t& v) override;
        bool Read(bool& v) override;
        bool Read(Serialize& obj) override;
        bool Read(uint8_t* data, uint16_t len) override;
        bool ReadFunctionId(uint8_t& functionId) override;

        uint8_t WriteMessageId() override;
        void WriteMessageId(uint8_t id) override;
        bool ReadMessageId(uint8_t& id) override;
    protected:
        virtual void WriteByte(uint8_t v) = 0;
        virtual bool ReadByte(uint8_t& v) = 0;

        bool ReadStartToken(uint8_t& interfaceId) override;
        void WriteMessage(const char* msg);
    private:
        void FlushInput();
        void ReportError(const char* msg);
        bool ReadByteNoSpaces(uint8_t& b);
        void WriteInternal(uint8_t v);
        bool ReadInternal(uint8_t& v);
        bool ReadInternalWithErrorReport(uint8_t& v);
        bool PeakInternal(uint8_t& v);
        bool PeakInternalWithErrorReport(uint8_t& v);
        
        void WriteSeperator();
        void WriteAscii(int32_t v);
        void WriteAscii(uint32_t v);
        void WriteAsciiU(uint32_t v);
        bool ReadAscii(int32_t& v);
        bool ReadAscii(uint32_t& v);
        bool ReadAsciiInsideString(uint32_t& v);
        bool ReadAsciiEnum(uint32_t& v);

        void WriteString(const char* string);
        void ReadCommand();
        bool pendingPeakByte = false;
        bool insideString = false;
        uint8_t readFunctionId = 0;
        uint8_t peakByte = 0;
        uint8_t seperatorIndex = 0;
        uint8_t outputInterfaceId = 0;
        bool outputHex = false;
        bool outputTrimRet = false;
    };
}
#endif
