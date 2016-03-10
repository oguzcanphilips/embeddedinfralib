#include "infra/stream/public/OutputStream.hpp"

namespace infra
{
    StreamWriter::StreamWriter()
        : softFail(false)
    {

    }

    StreamWriter::StreamWriter(SoftFail)
        : softFail(true)
    {

    }

    StreamWriter::~StreamWriter()
    {
        assert(checkedFail);
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
    {
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
        result.width = width.width;
        return result;
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
            Writer().Insert('-');
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
            Writer().Insert('0');

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
        for (std::size_t i = nofDigits; i < *width; ++i)
            Writer().Insert('0');

        while (mask)
        {
            Writer().Insert(static_cast<char>(hexChars[(v / mask) % 16]));
            mask /= 16;
        }
    }
}
