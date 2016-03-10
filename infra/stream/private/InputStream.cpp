#include "infra/stream/public/InputStream.hpp"
#include <cassert>

namespace infra
{
    InputStreamReader::InputStreamReader()
        : softFail(false)
    {

    }

    InputStreamReader::InputStreamReader(SoftFail)
        : softFail(true)
    {

    }

    InputStreamReader::~InputStreamReader()
    {
        assert(checkedFail);
    }

    bool InputStreamReader::Failed() const
    {
        checkedFail = true;
        return failed;
    }
    
    void InputStreamReader::ReportResult(bool ok)
    {
        if (!ok)
        {
            failed = true;
            assert(softFail);
        }
        checkedFail = !softFail;
    }
    
    InputStream::InputStream(InputStreamReader& reader)
        : reader(reader)
    {
    }

    bool InputStream::IsEmpty() const
    {
        return reader.Empty();
    }

    bool InputStream::IsFailed() const
    {
        return reader.Failed();
    }
    InputStreamReader& InputStream::Reader()
    {
        return reader;
    }

    DataInputStream::DataInputStream(InputStreamReader& reader)
        : InputStream(reader)
    {
    }
    
    TextInputStream DataInputStream::operator>>(Text)
    {
        return TextInputStream(Reader());
    }

    DataInputStream& DataInputStream::operator>>(ForwardStream forward)
    {
        Reader().Forward(forward.amount);
        return *this;
    }

    DataInputStream& DataInputStream::operator>>(ByteRange data)
    {
        Reader().Extract(data);
        return *this;
    }

    TextInputStream::TextInputStream(InputStreamReader& reader)
        : InputStream(reader)
    {
    }

    TextInputStream::TextInputStream(InputStreamReader& reader, SoftFail)
        : InputStream(reader)
    {
    }

    DataInputStream TextInputStream::operator>>(Data)
    {
        return DataInputStream(Reader());
    }

    TextInputStream TextInputStream::operator>>(Hex)
    {
        TextInputStream res(*this);
        res.isDecimal = false;
        return res;
    }
    TextInputStream TextInputStream::operator>>(Width width)
    {
        TextInputStream res(*this);
        res.width = width.width;
        return res;
    }
    TextInputStream& TextInputStream::operator>>(MemoryRange<char> text)
    {
        return *this;
    }
    TextInputStream& TextInputStream::operator>>(int8_t& v)
    {
        int32_t v32;
        Read(v32);
        v = v32;
        return *this;
    }
    TextInputStream& TextInputStream::operator>>(int16_t& v)
    {
        int32_t v32;
        Read(v32);
        v = v32;
        return *this;
    }
    TextInputStream& TextInputStream::operator>>(int32_t& v)
    {
        Read(v);
        return *this;
    }
    TextInputStream& TextInputStream::operator>>(uint8_t& v)
    {
        uint32_t v32;
        Read(v32);
        v = v32;
        return *this;
    }
    TextInputStream& TextInputStream::operator>>(uint16_t& v)
    {
        uint32_t v32;
        Read(v32);
        v = v32;
        return *this;
    }
    TextInputStream& TextInputStream::operator>>(uint32_t& v)
    {
        Read(v);
        return *this;
    }
    TextInputStream& TextInputStream::operator >> (const char* literal)
    {
        while (*literal)
        {
            uint8_t c;
            Reader().Extract(c);
            Reader().ReportResult(c == *literal);
            ++literal;
        }

        return *this;
    }
    void TextInputStream::SkipSpaces()
    {
        uint8_t c;
        Reader().Peek(c);
        while (c == ' ')
        {
            Reader().Extract(c);
            Reader().Peek(c);
        }
    }
    void TextInputStream::Read(int32_t& v)
    {
        if (isDecimal)
            ReadAsDecimal(v);
        else
            ReadAsHex(v);
    }

    void TextInputStream::Read(uint32_t& v)
    {
        if (isDecimal)
            ReadAsDecimal(v);
        else
            ReadAsHex(v);
    }

    void TextInputStream::ReadAsDecimal(int32_t& v)
    {
        SkipSpaces();
        uint8_t c;
        Reader().Peek(c);
        if (c == '-')
        {
            Reader().Extract(c);
            v = -1;
        }
        else
        {
            v = 1;
        }
        uint32_t vp;
        ReadAsDecimal(vp);
        v *= vp;
    }

    void TextInputStream::ReadAsDecimal(uint32_t& v)
    {
        SkipSpaces();

        v = 0;
        for (std::size_t i = 0; (i != width.ValueOr(std::numeric_limits<std::size_t>::max()) && !Reader().Empty()) || i == 0; ++i)
        {
            uint8_t c;
            Reader().Peek(c);

            if (c >= '0' && c <= '9')
                v = v * 10 + c - '0';
            else
            {
                Reader().ReportResult(i>0);
                break;
            }
            Reader().Extract(c);
        }
    }

    void TextInputStream::ReadAsHex(int32_t& v)
    {
        SkipSpaces();
        uint8_t c;
        Reader().Peek(c);
        if (c == '-')
        {
            Reader().Extract(c);
            v = -1;
        }
        else
        {
            v = 1;
        }
        uint32_t vp;
        ReadAsHex(vp);
        v *= vp;
    }

    void TextInputStream::ReadAsHex(uint32_t& v)
    {
        SkipSpaces();

        v = 0;

        for (std::size_t i = 0; (i != width.ValueOr(std::numeric_limits<std::size_t>::max()) && !Reader().Empty()) || i == 0; ++i)
        {
            uint8_t c;
            Reader().Peek(c);

            if (c >= '0' && c <= '9')
                v = (v << 4) + c - '0';
            else if (c >= 'a' && c <= 'f')
                v = (v << 4) + c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                v = (v << 4) + c - 'A' + 10;
            else
            {
                Reader().ReportResult(i>0);
                break;
            }

            Reader().Extract(c);
        }
    }
}