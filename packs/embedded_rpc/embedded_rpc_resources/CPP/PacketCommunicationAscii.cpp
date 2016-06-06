#include "PacketCommunicationAscii.hpp"
#include "Generated/CPP/PacketCommunicationLut.hpp"
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
            if (!Read(data[i]))
                return false;
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

    void PacketCommunicationAscii::FlushInput()
    {
        uint8_t v;
        while (ReadInternal(v) && v!='\n')
        {
        }
    }

    void PacketCommunicationAscii::PacketStart(uint8_t interfaceId, uint8_t functionId)
    {
        outputInterfaceId = interfaceId;
        if (outputTrimRet && (interfaceId & 0x01) == 0)
            return;

        for (const erpc::Lut::InterfaceSpec& iSpec : erpc::Lut::interfaceSpecs)
        {
            if (iSpec.id == interfaceId)
            {
                WriteString(iSpec.name);
                WriteByte('.');
                const uint16_t functionEnd = iSpec.functionIndex + iSpec.numberOfFuncions;
                for (uint16_t i = iSpec.functionIndex; i != functionEnd; ++i)
                {
                    if (erpc::Lut::functionSpecs[i].id == functionId)
                    {
                        WriteString(erpc::Lut::functionSpecs[i].name);
                        WriteByte('(');
                        return;
                    }
                }
            }
        }
    }

    void PacketCommunicationAscii::PacketDone()
    {
        if (!outputTrimRet || outputInterfaceId & 0x01)
            WriteByte(')');
        WriteByte('\n');
        seperatorIndex = 0;
    }

    bool PacketCommunicationAscii::ReadDone()
    {
        uint8_t c = 0;
        if (!ReadInternalWithErrorReport(c))
            return false;

        if (c != '\n')
        {
            ReportError("Syntax error");
            return false;
        }
        return true;
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
            case '$':
                if (readIndex == 0)
                {
                    ReadCommand();
                    return false;
                }
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
                    if (interfaceSpec == nullptr)
                    {
                        ReportError("Unknown interface");
                        FlushInput();
                        return false;
                    }
                    break;
            case '(':
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
                            if (!PeakInternalWithErrorReport(reinterpret_cast<uint8_t&>(c)))
                                return false;
                            if (c == ')') 
                                ReadInternalWithErrorReport(reinterpret_cast<uint8_t&>(c));
                            return true;
                        }
                    }
                    ReportError("Unknown function");
                    FlushInput();
                    return false;
                }
                break;
            default:
                input[readIndex++] = c;
                break;
            }
        }
        ReportError("Syntax error");
        FlushInput();
        return false;
    }

    void PacketCommunicationAscii::WriteSeperator()
    {
        if (seperatorIndex == 0)
            seperatorIndex++;
        else
            WriteByte(',');
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
        uint32_t base = outputHex ? 16 : 10;
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
        if (outputHex)
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

    bool PacketCommunicationAscii::ReadByteNoSpaces(uint8_t& b)
    {
        bool res = ReadByte(b);

        if (!insideString)
        while (res && (b == ' ' || b == '\r'))
            res = ReadByte(b);
        return res;
    }
    bool PacketCommunicationAscii::ReadInternal(uint8_t& v)
    {
        if (pendingPeakByte)
        {
            pendingPeakByte = false;
            v = peakByte;
            return true;
        }
        return ReadByteNoSpaces(v);
    }
    bool PacketCommunicationAscii::PeakInternal(uint8_t& v)
    {
        if (!pendingPeakByte)
            pendingPeakByte = ReadByteNoSpaces(peakByte);
        v = peakByte;
        return pendingPeakByte;
    }

    bool PacketCommunicationAscii::ReadAscii(int32_t& v)
    {
        uint8_t d = 0;
        if (!PeakInternalWithErrorReport(d))
            return false;

        if (d == '-')
        {
            ReadInternalWithErrorReport(d);
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

    void PacketCommunicationAscii::WriteMessage(const char* msg)
    {
        for (; *msg; ++msg)
            WriteInternal(*msg);
    }
    void PacketCommunicationAscii::ReportError(const char* msg)
    {
        WriteMessage("$ERROR:");
        WriteMessage(msg);
        WriteMessage("\n");
    }
    bool PacketCommunicationAscii::ReadInternalWithErrorReport(uint8_t& v)
    {
        if (ReadInternal(v))
            return true;

        ReportError("No input");
        return false;
    }
    bool PacketCommunicationAscii::PeakInternalWithErrorReport(uint8_t& v)
    {
        if (PeakInternal(v))
            return true;

        ReportError("No input");
        return false;
    }
    bool PacketCommunicationAscii::ReadAsciiInsideString(uint32_t& v)
    {
        uint8_t d = 0;
        if (!ReadInternalWithErrorReport(d))
            return false;
        if (d == '"')
        {
            v = 0;
            insideString = false;
            return ReadInternalWithErrorReport(d); // read seperator
        }
        else if (d == '\\')
        {
            if (!ReadInternalWithErrorReport(d))
                return false;
        }
        v = d;
        return true;
    }

    bool PacketCommunicationAscii::ReadAscii(uint32_t& v)
    {
        v = 0;
        uint8_t d = 0;

        if (insideString)
            return ReadAsciiInsideString(v);

        if (!ReadInternalWithErrorReport(d))
            return false;

        if (d == '"')
        {
            insideString = true;
            return ReadAsciiInsideString(v);
        }
        if (d == '\n')
        {
            ReportError("Unexpected end of line");
            return false;
        }

        bool found = false;
        bool isHex = false;
        uint8_t dv = 0;
        while (IsDigit(d, isHex, dv))
        {
            found = true;
            v *= isHex ? 16 : 10;
            v += dv;
            if (!ReadInternalWithErrorReport(d))
                return false;
            if (d == '\n')
            {
                ReportError("Unexpected end of line");
                return false;
            }
            if (d == 'x' || d == 'X')
            {
                isHex = true;
                if (v == 0)
                {
                    if (!ReadInternalWithErrorReport(d))
                        return false;
                }
                else
                {
                    ReportError("Syntax error");
                    return false;
                }
            }
        } 
        if (!found)
        {
            std::array<char, 20> enumInput;
            enumInput[0] = d;
            uint32_t readIndex = 1;
            char c = 0;
            while (!found && ReadInternalWithErrorReport(reinterpret_cast<uint8_t&>(c)))
            {
                if (c != ',' && c != ')')
                {
                    if (readIndex < (enumInput.size() - 1))
                        enumInput[readIndex++] = c;
                }
                else
                {
                    enumInput[readIndex] = 0;
                    for (const erpc::Lut::EnumSpec& enumSpec : erpc::Lut::enumSpecs)
                    {
                        if (strcmp(enumSpec.name, &enumInput[0]) == 0)
                        {
                            found = true;
                            v = enumSpec.id;
                            break;
                        }
                    }
                }
            }
        }
        return found;
    }

    void PacketCommunicationAscii::ReadCommand()
    {
        std::array<char, 10> input;
        uint32_t readIndex = 0;
        char c = 0;
        while (ReadInternalWithErrorReport(reinterpret_cast<uint8_t&>(c)))
        {
            if (c != '\n')
            {
                if (readIndex < (input.size()-1))
                    input[readIndex++] = c + ((c >= 'a' && c <= 'z') ? 'A' - 'a' : 0);
            }
            else
            {
                input[readIndex] = 0;
                if (strcmp(&input[0], "HEX") == 0)
                    outputHex = true;
                else if (strcmp(&input[0], "DEC") == 0)
                    outputHex = false;
                else if (strcmp(&input[0], "TRIM") == 0)
                    outputTrimRet = true;
                else if (strcmp(&input[0], "!TRIM") == 0)
                    outputTrimRet = false;
                else
                    ReportError("Unknown command");
                break;
            }
        }
    }
}
