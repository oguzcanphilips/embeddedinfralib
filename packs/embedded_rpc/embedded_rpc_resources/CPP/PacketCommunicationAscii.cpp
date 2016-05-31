#include "PacketCommunicationAscii.hpp"
#include "Generated/CPP/PacketHeaderLut.hpp"
#include <array>
#include <cstdlib>

namespace erpc
{
    PacketCommunicationAscii::PacketCommunicationAscii()
    {
    }
        
    void PacketCommunicationAscii::Write(uint8_t v)
    {
        Write(static_cast<uint32_t>(v));
    }

    void PacketCommunicationAscii::Write(uint16_t v)
    {
        Write(static_cast<uint32_t>(v));
    }

    void PacketCommunicationAscii::Write(uint32_t v)
    {
        WriteAscii(v);
    }

    void PacketCommunicationAscii::Write(const uint8_t* data, uint16_t len)
    {
        while (len--)
            Write(*data++);
    }

    void PacketCommunicationAscii::Write(const char* string)
    {
        WriteSeperator();
        WriteInternal('"');
        while (*string)
        {
            if (*string == '\\' || *string == '"')
                WriteInternal('\\');
            WriteInternal(*string++);
        }
        WriteInternal('"');
    }

    void PacketCommunicationAscii::Write(int8_t v)
    {
        Write(static_cast<int32_t>(v));
    }

    void PacketCommunicationAscii::Write(int16_t v)
    {
        Write(static_cast<int32_t>(v));
    }

    void PacketCommunicationAscii::Write(int32_t v)
    {
        WriteAscii(v);
    }

    void PacketCommunicationAscii::Write(bool v)
    {
        Write((uint8_t)(v ? 1 : 0));
    }

    void PacketCommunicationAscii::Write(const Serialize& obj)
    {
        obj.Write(*this);
    }
    
    bool PacketCommunicationAscii::Read(uint8_t& v)
    {
        uint32_t r;
        if (!Read(r))
            return false;

        v = static_cast<uint8_t>(r);
        return true;
    }

    bool PacketCommunicationAscii::Read(uint16_t& v)
    {
        uint32_t r;
        if (!Read(r)) 
            return false;

        v = static_cast<uint16_t>(r);
        return true;
    }

    bool PacketCommunicationAscii::Read(uint32_t& v)
    {
        return ReadAscii(v);
    }

    bool PacketCommunicationAscii::Read(bool& v)
    {
        uint8_t c;
        if (!Read(c)) return false;
        v = c ? true : false;
        return true;
    }

    bool PacketCommunicationAscii::Read(uint8_t* data, uint16_t len)
    {
        for (uint16_t i = 0; i < len; ++i)
        {
            if (!Read(data[i])) return false;
        }
        return true;
    }

    bool PacketCommunicationAscii::Read(int8_t& v)
    {
        int32_t r;
        if (!Read(r))
            return false;
        v = static_cast<int8_t>(r);
        return true;
    }

    bool PacketCommunicationAscii::Read(int16_t& v)
    {
        int32_t r;
        if (!Read(r))
            return false;
        v = static_cast<int16_t>(r);
        return true;
    }

    bool PacketCommunicationAscii::Read(int32_t& v)
    {
        return ReadAscii(v);
    }

    bool PacketCommunicationAscii::Read(Serialize& obj)
    {
        return obj.Read(*this);
    }

    uint8_t PacketCommunicationAscii::WriteMessageId()
    {
        return 0;
    }

    void PacketCommunicationAscii::WriteMessageId(uint8_t id)
    {
    }

    bool PacketCommunicationAscii::ReadMessageId(uint8_t& id)
    {
        id = 0;
        return true;
    }

    void PacketCommunicationAscii::PacketStart(uint8_t interfaceId, uint8_t functionId)
    {
        uint32_t interfaceSpecIndex = 0;
        for (const erpc::Lut::InterfaceSpec& iSpec : erpc::Lut::interfaceSpecs)
        {
            if (iSpec.id == interfaceId)
            {
                WriteSeperator();
                WriteString(iSpec.name);
                const uint16_t functionEnd = iSpec.functionIndex + iSpec.numberOfFuncions;
                for (uint16_t i = iSpec.functionIndex; i != functionEnd; ++i)
                {
                    if (erpc::Lut::functionSpecs[i].id == functionId)
                    {
                        WriteSeperator();
                        WriteString(erpc::Lut::functionSpecs[i].name);
                        return;
                    }
                }
            }
        }
    }

    void PacketCommunicationAscii::PacketDone()
    {
        if (seperatorIndex == 2)
            WriteByte('(');
        WriteByte(')');
        WriteByte('\n');
        seperatorIndex = 0;
    }

    bool PacketCommunicationAscii::ReadDone()
    {
        uint8_t c = 0;
        if (!ReadInternal(c))
            return false;

        if (c == ')')
            if (!ReadInternal(c))
                return false;

        return c == '\n';
    }

    bool PacketCommunicationAscii::ReadFunctionId(uint8_t& functionId)
    {
        functionId = readFunctionId;
        return true;
    }

    bool PacketCommunicationAscii::ReadStartToken(uint8_t& interfaceId)
    {
        insideString = false;
        // read string with format "iiii.ffff("

        const erpc::Lut::InterfaceSpec* interfaceSpec = nullptr;
        std::array<char, 100> input;
        uint32_t readIndex = 0;
        char c;
        while (readIndex < input.size() && ReadInternal(reinterpret_cast<uint8_t&>(c)))
        {
            switch (c)
            {
            case '\n':
                readIndex = 0;
                break;
            case '.':
                input[readIndex] = 0;
                readIndex = 0;
                for (const erpc::Lut::InterfaceSpec& iSpec : erpc::Lut::interfaceSpecs)
                {
                    if (strcmp(iSpec.name, &input[0]) == 0)
                    {
                        interfaceSpec = &iSpec;
                        break;
                    }
                }
                break;
            case '(':
                if (interfaceSpec == nullptr)
                    return false;
                input[readIndex] = 0;
                readIndex = 0;
                {
                    const uint16_t functionEnd = interfaceSpec->functionIndex + interfaceSpec->numberOfFuncions;
                    for (uint16_t i = interfaceSpec->functionIndex; i != functionEnd; ++i)
                    {
                        if (strcmp(erpc::Lut::functionSpecs[i].name, &input[0]) == 0)
                        {
                            interfaceId = interfaceSpec->id;
                            readFunctionId = erpc::Lut::functionSpecs[i].id;
                            return true;
                        }
                    }
                }
                break;
            default:
                input[readIndex++] = c;
                break;
            }
        }
        return false;
    }

    void PacketCommunicationAscii::WriteSeperator()
    {
        switch (seperatorIndex)
        {
        case 0:
            seperatorIndex++;
            break;
        case 1:
            seperatorIndex++;
            WriteByte('.');
            break;
        case 2:
            seperatorIndex++;
            WriteByte('(');
            break;
        default:
            WriteByte(',');
            break;
        }
    }
    void PacketCommunicationAscii::WriteAscii(int32_t v)
    {
        WriteSeperator();
        if (v < 0)
        {
            WriteByte('-');
            WriteAsciiU(static_cast<uint32_t>(-v));
        }
        else
        {
            WriteAsciiU(static_cast<uint32_t>(v));
        }
    }

    void PacketCommunicationAscii::WriteAscii(uint32_t v)
    {
        WriteSeperator();
        WriteAsciiU(v);
    }

    void PacketCommunicationAscii::WriteAsciiU(uint32_t v)
    {
        bool isHex = false;
        uint32_t base = isHex ? 16 : 10;
        const static uint8_t digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
        uint8_t str[10];
        uint8_t index = sizeof(str)-1;
        str[index] = 0;
        do
        {
            index--;
            str[index] = digits[v % base];
            v /= base;
        } 
        while (v);
        if (isHex)
            WriteString("0x");

        WriteString(reinterpret_cast<const char*>(str+index));
    }
    void PacketCommunicationAscii::WriteString(const char* string)
    {
        for (; *string; ++string)
            WriteInternal(*string);
    }

    void PacketCommunicationAscii::WriteInternal(uint8_t v)
    {
        WriteByte(v);
    }

    bool PacketCommunicationAscii::ReadInternal(uint8_t& v)
    {
        if (pendingPeakByte)
        {
            pendingPeakByte = false;
            v = peakByte;
            return true;
        }
        return ReadByte(v);
    }

    bool PacketCommunicationAscii::PeakInternal(uint8_t& v)
    {
        if (!pendingPeakByte)
            pendingPeakByte = ReadByte(peakByte);
        v = peakByte;
        return pendingPeakByte;
    }

    bool PacketCommunicationAscii::ReadAscii(int32_t& v)
    {
        uint8_t d = 0;
        if (!PeakInternal(d))
            return false;

        if (d == '-')
        {
            ReadInternal(d);
            uint32_t uv = 0;
            if (!ReadAscii(uv))
                return false;
            v = -static_cast<int32_t>(uv);
        }
        else
        {
            uint32_t uv = 0;
            if (!ReadAscii(uv))
                return false;
            v = uv;
        }
        return true;
    }

    static bool IsDigit(char d, bool isHex, uint8_t& value)
    {
        if (d >= '0' && d <= '9')
        {
            value = d - '0';
            return true;
        }
        if (isHex)
        {
            if (d >= 'A' && d <= 'F')
            {
                value = d - 'A' + 10;
                return true;
            }
            if (d >= 'a' && d <= 'f')
            {
                value = d - 'a' + 10;
                return true;
            }
        }
        return false;
    }

    bool PacketCommunicationAscii::ReadAscii(uint32_t& v)
    {
        bool isHex = false;
        v = 0;
        uint8_t d = 0;
        uint8_t dv = 0;

        if (insideString)
        {
            if (!ReadInternal(d))
                return false;
            if (d == '"')
            {
                v = 0;
                insideString = false;
                return ReadInternal(d); // read seperator
            }
            else if (d == '\\')
            {
                if (!ReadInternal(d))
                    return false;
            }
            v = d;
            return true;
        }
        do
        {
            if (!ReadInternal(d) || d == '\n')
                return false;
            if (d == '"')
            {
                insideString = true;
                return ReadAscii(v);
            }

        } 
        while (!IsDigit(d, isHex, dv));
        do
        {
            v *= isHex ? 16 : 10;
            v += dv;
            if (!PeakInternal(d))
                return false;
            if (d == '\n')
                return true;
            if (d == 'x' || d == 'X')
            {
                if (v == 0)
                {
                    ReadInternal(d);
                    isHex = true;
                }
            }
            if (!ReadInternal(d))
                return false;
        }
        while (IsDigit(d, isHex, dv));
        return true;
    }
}