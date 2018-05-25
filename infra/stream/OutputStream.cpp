#include "infra/stream/OutputStream.hpp"
#include <cmath>
#include <limits>

namespace infra
{
    const uint8_t* StreamWriter::ConstructSaveMarker() const
    {
        std::abort();
    }

    std::size_t StreamWriter::GetProcessedBytesSince(const uint8_t* marker) const
    {
        std::abort();
    }

    infra::ByteRange StreamWriter::SaveState(const uint8_t* marker)
    {
        std::abort();
    }

    void StreamWriter::RestoreState(infra::ByteRange range)
    {
        std::abort();
    }

    StreamWriterDummy::StreamWriterDummy()
    {}

    void StreamWriterDummy::Insert(ConstByteRange range, StreamErrorPolicy& errorPolicy)
    {}

    std::size_t StreamWriterDummy::Available() const
    {
        return std::numeric_limits<std::size_t>::max();
    }

    OutputStream::OutputStream(StreamWriter& writer, StreamErrorPolicy& errorPolicy)
        : writer(writer)
        , errorPolicy(errorPolicy)
    {}

    bool OutputStream::Failed() const
    {
        return errorPolicy.Failed();
    }

    const uint8_t* OutputStream::SaveMarker() const
    {
        return writer.ConstructSaveMarker();
    }

    std::size_t OutputStream::ProcessedBytesSince(const uint8_t* marker) const
    {
        return writer.GetProcessedBytesSince(marker);
    }

    std::size_t OutputStream::Available() const
    {
        return writer.Available();
    }
    
    StreamWriter& OutputStream::Writer() const
    {
        return writer;
    }
    
    StreamErrorPolicy& OutputStream::ErrorPolicy() const
    {
        return errorPolicy;
    }

    TextOutputStream DataOutputStream::operator<<(Text)
    {
        return TextOutputStream(Writer(), ErrorPolicy());
    }

    TextOutputStream& TextOutputStream::operator<<(const char* zeroTerminatedString)
    {
        Writer().Insert(ReinterpretCastByteRange(MakeRange(zeroTerminatedString, zeroTerminatedString + std::strlen(zeroTerminatedString))), ErrorPolicy());

        return *this;
    }
    TextOutputStream& TextOutputStream::operator<<(BoundedConstString string)
    {
        Writer().Insert(ReinterpretCastByteRange(MakeRange(string.begin(), string.end())), ErrorPolicy());

        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(const std::string& string)
    {
        Writer().Insert(ReinterpretCastByteRange(MakeRange(string.data(), string.data() + string.size())), ErrorPolicy());

        return *this;
    }

    DataOutputStream TextOutputStream::operator<<(Data)
    {
        return DataOutputStream(Writer(), ErrorPolicy());
    }

    TextOutputStream TextOutputStream::operator<<(Hex)
    {
        TextOutputStream result(*this);
        result.radix = Radix::hex;
        return result;
    }

    TextOutputStream TextOutputStream::operator<<(Bin)
    {
        TextOutputStream result(*this);
        result.radix = Radix::bin;
        return result;
    }

    TextOutputStream TextOutputStream::operator<<(Width width)
    {
        TextOutputStream result(*this);
        result.width = width;
        return result;
    }

    TextOutputStream& TextOutputStream::operator<<(Endl)
    {
        *this << "\r\n";
        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(char c)
    {
        Writer().Insert(infra::MakeByteRange(c), ErrorPolicy());
        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(uint8_t v)
    {
        switch (radix)
        {
            case Radix::dec:
                OutputAsDecimal(v);
                break;
            case Radix::bin:
                OutputAsBinary(v);
                break;
            case Radix::hex:
                OutputAsHexadecimal(v);
                break;
            default:
                std::abort();
        }

        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(int32_t v)
    {
        if (v < 0)
            Writer().Insert(infra::MakeByteRange('-'), ErrorPolicy());
        switch (radix)
        {
            case Radix::dec:
                OutputAsDecimal(std::abs(v));
                break;
            case Radix::bin:
                OutputAsBinary(std::abs(v));
                break;
            case Radix::hex:
                OutputAsHexadecimal(std::abs(v));
                break;
            default:
                std::abort();
        }

        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(uint32_t v)
    {
        switch (radix)
        {
            case Radix::dec:
                OutputAsDecimal(v);
                break;
            case Radix::bin:
                OutputAsBinary(v);
                break;
            case Radix::hex:
                OutputAsHexadecimal(v);
                break;
            default:
                std::abort();
        }

        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(int64_t v)
    {
        if (v < 0)
            Writer().Insert(infra::MakeByteRange('-'), ErrorPolicy());
        switch (radix)
        {
            case Radix::dec:
                OutputAsDecimal(std::abs(v));
                break;
            case Radix::bin:
                OutputAsBinary(std::abs(v));
                break;
            case Radix::hex:
                OutputAsHexadecimal(std::abs(v));
                break;
            default:
                std::abort();
        }

        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(uint64_t v)
    {
        switch (radix)
        {
            case Radix::dec:
                OutputAsDecimal(v);
                break;
            case Radix::bin:
                OutputAsBinary(v);
                break;
            case Radix::hex:
                OutputAsHexadecimal(v);
                break;
            default:
                std::abort();
        }

        return *this;
    }

#ifndef _MSC_VER                                                                                                    //TICS !POR#021
    TextOutputStream& TextOutputStream::operator<<(int v)
    {
        if (v < 0)
            Writer().Insert(infra::MakeByteRange('-'), ErrorPolicy());
        switch (radix)
        {
            case Radix::dec:
                OutputAsDecimal(std::abs(v));
                break;
            case Radix::bin:
                OutputAsBinary(std::abs(v));
                break;
            case Radix::hex:
                OutputAsHexadecimal(std::abs(v));
                break;
            default:
                std::abort();
        }

        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(unsigned int v)
    {
        switch (radix)
        {
            case Radix::dec:
                OutputAsDecimal(v);
                break;
            case Radix::bin:
                OutputAsBinary(v);
                break;
            case Radix::hex:
                OutputAsHexadecimal(v);
                break;
            default:
                std::abort();
        }

        return *this;
    }
#endif

    TextOutputStream& TextOutputStream::operator<<(float v)
    {
        if (v < 0)
            *this << "-";

        v = std::abs(v);

        *this << static_cast<uint32_t>(v);
        v -= static_cast<uint32_t>(v);
        *this << ".";
        *this << infra::Width(3,'0') << static_cast<uint32_t>(v * 1000);
        return *this;
    }

    void TextOutputStream::OutputAsDecimal(uint64_t v)
    {
        uint64_t nofDigits = 1;
        uint64_t mask = 1;

        while (v / mask >= 10)
        {
            mask *= 10;
            ++nofDigits;
        }

        if (width)
            for (uint64_t i = nofDigits; i < width->width; ++i)
                Writer().Insert(infra::MakeByteRange(width->padding), ErrorPolicy());

        while (mask != 0)
        {
            Writer().Insert(infra::MakeByteRange(static_cast<char>(((v / mask) % 10) + '0')), ErrorPolicy());
            mask /= 10;
        }
    }

    void TextOutputStream::OutputAsBinary(uint64_t v)
    {
        uint64_t nofDigits = 1;
        uint64_t mask = 1;

        while (v / mask >= 2)
        {
            mask *= 2;
            ++nofDigits;
        }

        if (width)
            for (uint64_t i = nofDigits; i < width->width; ++i)
                Writer().Insert(infra::MakeByteRange(width->padding), ErrorPolicy());

        while (mask != 0)
        {
            Writer().Insert(infra::MakeByteRange(static_cast<char>('0' + ((v / mask) & 1))), ErrorPolicy());
            mask /= 2;
        }
    }

    void TextOutputStream::OutputAsHexadecimal(uint64_t v)
    {
        static const char hexChars[] = "0123456789abcdef";

        uint64_t nofDigits = 1;
        uint64_t mask = 1;

        while (v / mask >= 16)
        {
            mask *= 16;
            ++nofDigits;
        }

        if (width)
            for (uint64_t i = nofDigits; i < width->width; ++i)
                Writer().Insert(infra::MakeByteRange(width->padding), ErrorPolicy());

        while (mask != 0)
        {
            Writer().Insert(infra::MakeByteRange(hexChars[(v / mask) % 16]), ErrorPolicy());
            mask /= 16;
        }
    }

    void TextOutputStream::FormatArgs(const char* format, infra::MemoryRange<FormatterBase*> formatters)
    {
        while (*format != '\0')
        {
            const char* start = format;
            while (*format != '\0' && *format != '%')
                ++format;

            Writer().Insert(infra::ReinterpretCastByteRange(infra::MakeRange(start, format)), ErrorPolicy());

            if (*format == '%')
            {
                if (format[1] >= '1' && format[1] <= '9')
                {
                    if (format[2] == '%')
                    {
                        uint8_t index = format[1] - '1';
                        if (index < formatters.size())
                            formatters[index]->Stream(*this);
                        ++format;
                    }
                    ++format;
                }

                ++format;
            }
        }
    }

    DataOutputStream::WithErrorPolicy::WithErrorPolicy(StreamWriter& writer)
        : DataOutputStream(writer, errorPolicy)
    {}

    DataOutputStream::WithErrorPolicy::WithErrorPolicy(StreamWriter& writer, SoftFail)
        : DataOutputStream(writer, errorPolicy)
        , errorPolicy(softFail)
    {}

    DataOutputStream::WithErrorPolicy::WithErrorPolicy(StreamWriter& writer, NoFail)
        : DataOutputStream(writer, errorPolicy)
        , errorPolicy(softFail)
    {}

    DataOutputStream::WithErrorPolicy::WithErrorPolicy(const WithErrorPolicy& other)
        : DataOutputStream(other.Writer(), errorPolicy)
        , errorPolicy(other.ErrorPolicy())
    {}

    TextOutputStream::WithErrorPolicy::WithErrorPolicy(StreamWriter& writer)
        : TextOutputStream(writer, errorPolicy)
    {}

    TextOutputStream::WithErrorPolicy::WithErrorPolicy(StreamWriter& writer, SoftFail)
        : TextOutputStream(writer, errorPolicy)
        , errorPolicy(softFail)
    {}

    TextOutputStream::WithErrorPolicy::WithErrorPolicy(StreamWriter& writer, NoFail)
        : TextOutputStream(writer, errorPolicy)
        , errorPolicy(noFail)
    {}

    TextOutputStream::WithErrorPolicy::WithErrorPolicy(const WithErrorPolicy& other)
        : TextOutputStream(other.Writer(), errorPolicy)
        , errorPolicy(other.ErrorPolicy())
    {}

    AsAsciiHelper::AsAsciiHelper(infra::ConstByteRange data)
        : data(data)
    {}

    infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const AsAsciiHelper& asAsciiHelper)
    {
        for (uint8_t byte : asAsciiHelper.data)
            if (byte < 32 || byte >= 128)
                stream << '.';
            else
                stream << static_cast<char>(byte);

        return stream;
    }

    AsHexHelper::AsHexHelper(infra::ConstByteRange data)
        : data(data)
    {}

    infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const AsHexHelper& asHexHelper)
    {
        infra::TextOutputStream hexStream = stream << hex << Width(2, '0');

        for (uint8_t byte : asHexHelper.data)
            hexStream << byte;

        return stream;
    }

    AsBase64Helper::AsBase64Helper(infra::ConstByteRange data)
        : data(data)
    {}

    infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const AsBase64Helper& asHexHelper)
    {
        static const char* encodeTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        uint8_t bitIndex = 2;                                                                                           //TICS !OLC#006
        uint8_t encodedByte = 0;
        uint32_t size = 0;

        for (uint8_t byte : asHexHelper.data)
        {
            encodedByte |= byte >> bitIndex;
            stream << encodeTable[encodedByte];
            ++size;

            encodedByte = static_cast<uint8_t>(byte << (8 - bitIndex)) >> 2;                                            //TICS !POR#006

            bitIndex += 2;

            if (bitIndex == 8)
            {
                stream << encodeTable[encodedByte];
                ++size;
                encodedByte = 0;
                bitIndex = 2;
            }
        }

        if ((size & 3) != 0)
        {
            stream << encodeTable[encodedByte];
            ++size;
        }
        if ((size & 3) != 0)
        {
            stream << '=';
            ++size;
        }
        if ((size & 3) != 0)
        {
            stream << '=';
            ++size;
        }

        return stream;
    }

    infra::TextOutputStream& operator<<(infra::TextOutputStream&& stream, const AsBase64Helper& asBase64Helper)
    {
        return stream << asBase64Helper;
    }

    AsAsciiHelper AsAscii(infra::ConstByteRange data)
    {
        return AsAsciiHelper(data);
    }

    AsHexHelper AsHex(infra::ConstByteRange data)
    {
        return AsHexHelper(data);
    }

    AsBase64Helper AsBase64(infra::ConstByteRange data)
    {
        return AsBase64Helper(data);
    }
}
