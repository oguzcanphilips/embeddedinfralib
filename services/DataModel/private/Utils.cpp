#include "Utils.hpp"
#include <string.h>

void util::Replace(char* b, uint32_t blen, char* pos, uint32_t len, const char* replace)
{
    if (pos == 0) return;

    uint32_t replaceLen = strlen(replace);
    uint32_t origLen = strlen(b);
    uint32_t resultLen = origLen;

    resultLen -= len;
    resultLen += replaceLen;
    if (resultLen > (blen - 1))
    {
        b[0] = 0;
        return;
    }
    if (len > replaceLen)
    {
        char* src = pos + len;
        char* dest = pos + replaceLen;
        while (*src)
        {
            *dest++ = *src++;
        }
    }
    else if (len < replaceLen)
    {
        char* srcStart = pos + len - 1;
        char* src = &b[origLen];
        char* dest = &b[resultLen];
        while (src != srcStart)
        {
            *dest-- = *src--;
        }
    }
    memcpy(pos, replace, replaceLen);
    b[resultLen + 1] = 0;
}

void util::Replace(char* b, uint32_t blen, const char* find, const char* replace)
{
    char* pos = strstr(b, find);
    if (pos == 0) return;

    uint32_t findLen = strlen(find);
    Replace(b, blen, pos, findLen, replace);
}

uint8_t util::HexChar2Nibble(char c)
{
  if ( (c >= '0') && (c <= '9') ) return c - '0';
  if ( (c >= 'a') && (c <= 'f') ) return c + 10 - 'a';
  if ( (c >= 'A') && (c <= 'F') ) return c + 10 - 'A';
  return 255;
}

bool util::Ascii2HexChar(char c1, char c2, uint8_t& out)
{
    out = HexChar2Nibble(c1);
    if (out == 255)
        return false;

    out <<= 4;
    uint8_t temp = HexChar2Nibble(c2);
    if (temp == 0xff)
        return false;

    out |= temp;
    return true;
}

util::OutputStream::OutputStream()
: mode(Dec)
, numberSize(0)
, endLineIndicator("")
{
}

void util::OutputStream::EndlIndicator(const char* indicator)
{
    endLineIndicator = indicator;
}

util::OutputStream& util::OutputStream::operator<<(const NumberSize& ns)
{
	numberSize = ns;
	return *this;
}

util::OutputStream& util::OutputStream::operator<<(OutputMode outputMode)
{
	mode = outputMode;
	return *this;
}
#ifndef WIN32
util::OutputStream& util::OutputStream::operator<<(int v)
{
	*this << (int32_t)v;
	return *this;
}
#endif

util::OutputStream& util::OutputStream::operator<<(Endline /*e*/)
{
    *this << endLineIndicator << "\r\n";
    return *this;
}

util::OutputStream& util::OutputStream::operator<<(bool v)
{
    if (v) ProcessOutput('1');
    else   ProcessOutput('0');
	return *this;
}

util::OutputStream& util::OutputStream::operator<<(char v)
{
    ProcessOutput(v);
    return *this;
}

util::OutputStream& util::OutputStream::operator<<(int8_t v)
{
	*this << (int32_t)v;
	return *this;
}

util::OutputStream& util::OutputStream::operator<<(int16_t v)
{
	*this << (int32_t)v;
	return *this;
}

util::OutputStream& util::OutputStream::operator<<(uint8_t v)
{
	*this << (uint32_t)v;
	return *this;
}

util::OutputStream& util::OutputStream::operator<<(uint16_t v)
{
	*this << (uint32_t)v;
	return *this;
}

util::OutputStream& util::OutputStream::operator<<(int32_t v)
{
	uint32_t vv = 0;
	if(v<0)
	{
        ProcessOutput('-');
		vv = -v;
	}
	else
	{
		vv = v;
	}
	*this << vv;
	return *this;
}

util::OutputStream& util::OutputStream::operator<<(uint32_t v)
{
	if(mode == Hex)
		AddHex(v);
	else
		AddDec(v);
    mode = Dec;
    numberSize = NumberSize();
	return *this;
}

util::OutputStream& util::OutputStream::operator<<(float v)
{
    float vv = 0;
    if (v<0)
    {
        ProcessOutput('-');
        vv = -v;
    }
    else
        vv = v;

    *this << static_cast<uint32_t>(vv);
    vv -= static_cast<uint32_t>(vv);
    ProcessOutput('.');
    *this << NumberSize(3) << static_cast<uint32_t>(vv*1000);
    return *this;
}


util::OutputStream& util::OutputStream::operator<<(const char* txt)
{
	for(;*txt;++txt)
        ProcessOutput(*txt);
	return *this;
}

void util::OutputStream::AddDec(uint32_t v)
{
	uint32_t nofDigits = 1;
	uint32_t mask = 1;
	while (v / mask>9)
	{
		mask *= 10;
		++nofDigits;
	}
	if(nofDigits>0)
		for(uint32_t i=nofDigits; i<numberSize.size;++i)
            ProcessOutput(numberSize.padding);
	while (mask)
	{
        ProcessOutput(((v / mask) % 10) + '0');
		mask /= 10;
	}
}

void util::OutputStream::AddHex(uint32_t v)
{
	uint32_t nofDigits = 1;
	uint32_t mask = 1;
	while (v / mask>15)
	{
		++nofDigits;
		mask *= 16;
	}
	if(nofDigits>0)
		for(uint32_t i=nofDigits; i<numberSize.size;++i)
            ProcessOutput(numberSize.padding);
	while (mask)
	{
		uint32_t d = (v / mask) % 16;
        if (d<10) ProcessOutput(static_cast<char>(d + '0'));
        else ProcessOutput(static_cast<char>(d - 10 + 'A'));
		mask /= 16;
	}
}

util::InputStream::InputStream()
{
}

bool util::InputStream::Fail() const
{
    return outOfData && peek == -1;
}

#ifndef WIN32
util::InputStream& util::InputStream::operator>>(int& v)
{
    int32_t v32 = v;
    operator>>(v32);
    v = v32;
    return *this;
}
#endif
util::InputStream& util::InputStream::operator>>(bool& v)
{
    char c = Read();
    v = (c == '1');
    return *this;
}
util::InputStream& util::InputStream::operator>>(char& v)
{
    v = Read();
    return *this;
}
util::InputStream& util::InputStream::operator>>(int8_t& v)
{
    int32_t v32 = v;
    operator>>(v32);
    v = static_cast<int8_t>(v32);
    return *this;
}
util::InputStream& util::InputStream::operator>>(uint8_t& v)
{
    uint32_t v32 = v;
    operator>>(v32);
    v = static_cast<uint8_t>(v32);
    return *this;
}
util::InputStream& util::InputStream::operator>>(int16_t& v)
{
    int32_t v32 = v;
    operator>>(v32);
    v = static_cast<int16_t>(v32);
    return *this;
}
util::InputStream& util::InputStream::operator>>(uint16_t& v)
{
    uint32_t v32 = v;
    operator>>(v32);
    v = static_cast<uint16_t>(v32);
    return *this;
}
util::InputStream& util::InputStream::operator>>(int32_t& v)
{
    char c = Peek();
    while ((c<'0' || c>'9') && c != '-')
    {
        c = Read();
        if (c == 0)
            return *this;
        c = Peek();
    }
    if (c == '-')
    {
        Read();
        uint32_t vu = 0;
        operator>>(vu);
        v = -1 * vu;
    }
    else
    {
        uint32_t vu = 0;
        operator>>(vu);
        v = vu;
    }
    return *this;
}

util::InputStream& util::InputStream::operator>>(uint32_t& v)
{
    char c = Peek();
    while (c<'0' || c>'9')
    {
        c = Read();
        if (c == 0)
            return *this;
        c = Peek();
    }
    v = 0;
    while (c >= '0' && c<='9')
    {
        v = v * 10 + (c - '0');
        c = Read();
        c = Peek();
    }
    return *this;
}

util::InputStream& util::InputStream::operator>>(int64_t& v)
{
    char c = Peek();
    while ((c<'0' || c>'9') && c != '-')
    {
        c = Read();
        if (c == 0)
            return *this;
        c = Peek();
    }
    if (c == '-')
    {
        Read();
        uint64_t vu = 0;
        operator>>(vu);
        v = -1 * vu;
    }
    else
    {
        uint64_t vu = 0;
        operator>>(vu);
        v = vu;
    }
    return *this;
}

util::InputStream& util::InputStream::operator>>(uint64_t& v)
{
    bool isNegative = false;
    char c = Peek();
    while ((c<'0' || c>'9') && c != '-')
    {
        c = Read();
        if (c == 0)
            return *this;
        c = Peek();
    }
    if (c == '-')
    {
        isNegative = true;
        c = Read();
        c = Peek();
    }
    v = 0;
    while (c >= '0' && c<='9')
    {
        v = v * 10 + (c - '0');
        c = Read();
        c = Peek();
    }
    if (isNegative)
        v *= -1;

    return *this;
}

util::InputStream& util::InputStream::operator>>(float& v)
{
    int32_t integer = 0;
    operator>>(integer);
    uint32_t frac = 0;
    int32_t div = (integer < 0) ? -1 : 1;
    if (Peek() == '.')
    {
        Read();
        char c = Peek();
        while (c >= '0' && c<='9')
        {
            div *= 10;
            frac = frac * 10 + (c - '0');
            c = Read();
            c = Peek();
        }
    }
    v = (float)frac;
    v /= div;
    v += integer;
    return *this;
}

char util::InputStream::Read()
{
    if (peek != -1)
    {
        char c = (char)peek;
        peek = -1;
        return c;
    }

    if (outOfData)
        return 0;
    
    char c = GetInput();
    if (c == 0)
        outOfData = true;

    return c;
}

char util::InputStream::Peek()
{
    if (peek != -1)
        return (char)peek;

    if (outOfData)
        return 0;

    peek = GetInput();
    if (peek == 0)
        outOfData = true;

    return (char)peek;
}
