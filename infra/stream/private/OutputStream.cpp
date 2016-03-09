#include "infra/stream/public/InputOutputStreamHelpers.hpp"

namespace infra
{
    DataOutputStream::DataOutputStream(OutputStreamWriter& writer)
        : writer(writer)
    {}

    TextOutputStream DataOutputStream::operator<<(Text)
    {
        return TextOutputStream(writer);
    }

    TextOutputStream::TextOutputStream(OutputStreamWriter& writer)
        : writer(writer)
    {}

    TextOutputStream& TextOutputStream::operator<<(const char* zeroTerminatedString)
    {
        writer.Insert(ReinterpretCastByteRange(MakeRange(zeroTerminatedString, zeroTerminatedString + std::strlen(zeroTerminatedString))));

        return *this;
    }

    DataOutputStream TextOutputStream::operator<<(Data)
    {
        return DataOutputStream(writer);
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
        result.width = width.width;
        return result;
    }

    TextOutputStream& TextOutputStream::operator<<(char c)
    {
        writer.Insert(c);

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

    TextOutputStream& TextOutputStream::operator<<(uint32_t v)
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
            writer.Insert('-');
        if (decimal)
            OutputAsDecimal(std::abs(v));
        else
            OutputAsHex(std::abs(v));

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
        for (std::size_t i = nofDigits; i < *width; ++i)
            writer.Insert('0');

        while (mask)
        {
            writer.Insert(static_cast<char>(((v / mask) % 10) + '0'));
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
        for (std::size_t i = nofDigits; i < *width; ++i)
            writer.Insert('0');

        while (mask)
        {
            writer.Insert(static_cast<char>(hexChars[(v / mask) % 16]));
            mask /= 16;
        }
    }

}
