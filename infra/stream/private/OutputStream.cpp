#include "infra/stream/public/OutputStream.hpp"

namespace infra
{
    StreamWriter::StreamWriter()
        : softFail(false)
    {}

    StreamWriter::StreamWriter(SoftFail)
        : softFail(true)
    {}

    StreamWriter::~StreamWriter()
    {
        assert(checkedFail);
    }

    void StreamWriter::SetSoftFail(bool enabled)
    {
        softFail = enabled;
    }

    bool StreamWriter::Failed() const
    {
        checkedFail = true;
        return failed;
    }

    void StreamWriter::ReportResult(bool ok)
    {
        if (!ok)
        {
            failed = true;
            assert(softFail);
        }
        checkedFail = !softFail;
    }

    OutputStream::OutputStream(StreamWriter& writer)
        : writer(writer)
    {}
    
    void OutputStream::SetSoftFail(bool enabled)
    {
        writer.SetSoftFail(enabled);
    }

    bool OutputStream::HasFailed() const
    {
        return writer.Failed();
    }

    StreamWriter& OutputStream::Writer()
    {
        return writer;
    }

    DataOutputStream::DataOutputStream(StreamWriter& writer)
        : OutputStream(writer)
    {}

    TextOutputStream DataOutputStream::operator<<(Text)
    {
        return TextOutputStream(Writer());
    }

    DataOutputStream& DataOutputStream::operator<<(ForwardStream forward)
    {
        Writer().Forward(forward.amount);
        return *this;
    }

    TextOutputStream::TextOutputStream(StreamWriter& writer)
        : OutputStream(writer)
    {}

    TextOutputStream& TextOutputStream::operator<<(const char* zeroTerminatedString)
    {
        Writer().Insert(ReinterpretCastByteRange(MakeRange(zeroTerminatedString, zeroTerminatedString + std::strlen(zeroTerminatedString))));

        return *this;
    }
    TextOutputStream& TextOutputStream::operator<<(BoundedConstString string)
    {
        Writer().Insert(ReinterpretCastByteRange(MakeRange(string.begin(), string.end())));

        return *this;
    }

    DataOutputStream TextOutputStream::operator<<(Data)
    {
        return DataOutputStream(Writer());
    }

    TextOutputStream TextOutputStream::operator<<(Hex)
    {
        TextOutputStream result(*this);
        result.decimal = false;
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
        Writer().Insert(c);
        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(uint8_t v)
    {
        if (decimal)
            OutputAsDecimal(v);
        else
            OutputAsHex(v);

        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(int32_t v)
    {
        if (v < 0)
            Writer().Insert('-');
        if (decimal)
            OutputAsDecimal(std::abs(v));
        else
            OutputAsHex(std::abs(v));

        return *this;
    }

    TextOutputStream& TextOutputStream::operator<<(uint32_t v)
    {
        if (decimal)
            OutputAsDecimal(v);
        else
            OutputAsHex(v);

        return *this;
    }

#ifndef _MSC_VER
    TextOutputStream& TextOutputStream::operator<<(int v)
    {
        if (v < 0)
            Writer().Insert('-');
        if (decimal)
            OutputAsDecimal(std::abs(v));
        else
            OutputAsHex(std::abs(v));

        return *this;
    }
#endif

    TextOutputStream& TextOutputStream::operator<<(float v)
    {
        float vv = 0;
        if (v < 0)
        {
            *this << "-";
            vv = -v;
        }
        else
            vv = v;

        *this << static_cast<uint32_t>(vv);
        vv -= static_cast<uint32_t>(vv);
        *this << ".";
        *this << infra::Width(3,'0') << static_cast<uint32_t>(vv * 1000);
        return *this;
    }

    void TextOutputStream::OutputAsDecimal(uint32_t v)
    {
        uint32_t nofDigits = 1;
        uint32_t mask = 1;

        while (v / mask >= 10)
        {
            mask *= 10;
            ++nofDigits;
        }

        if (width)
          for (std::size_t i = nofDigits; i < width->width; ++i)
                Writer().Insert(width->padding);

        while (mask)
        {
            Writer().Insert(static_cast<char>(((v / mask) % 10) + '0'));
            mask /= 10;
        }
    }

    void TextOutputStream::OutputAsHex(uint32_t v)
    {
        static const char hexChars[] = "0123456789abcdef";

        uint32_t nofDigits = 1;
        uint32_t mask = 1;

        while (v / mask >= 16)
        {
            mask *= 16;
            ++nofDigits;
        }

        if (width)
            for (std::size_t i = nofDigits; i < width->width; ++i)
                Writer().Insert(width->padding);

        while (mask)
        {
            Writer().Insert(static_cast<char>(hexChars[(v / mask) % 16]));
            mask /= 16;
        }
    }

    void TextOutputStream::FormatArgs(const char* format, infra::MemoryRange<FormatterBase*> formatters)
    {
        while (*format)
        {
            const char* start = format;
            while (*format && *format != '%')
                ++format;

            Writer().Insert(infra::ReinterpretCastByteRange(infra::MakeRange(start, format)));

            if (*format == '%')
            {
                if (format[1] >= '1' && format[1] <= '9')
                {
                    uint8_t index = format[1] - '1';
                    if (format[2] == '%')
                    {
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

    AsAsciiHelper::AsAsciiHelper(infra::ConstByteRange data)
        : data(data)
    {}

    infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const AsAsciiHelper& asAsciiHelper)
    {
        for (uint8_t byte : asAsciiHelper.data)
            if (byte < 32)
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

    AsAsciiHelper AsAscii(infra::ConstByteRange data)
    {
        return AsAsciiHelper(data);
    }

    AsHexHelper AsHex(infra::ConstByteRange data)
    {
        return AsHexHelper(data);
    }
}
