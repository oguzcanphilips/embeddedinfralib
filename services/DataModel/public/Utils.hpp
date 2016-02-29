#pragma once
#include <stdint.h>

namespace util
{
    void Replace(char* b, uint32_t blen, char* pos, uint32_t len, const char* replace);
    void Replace(char* b, uint32_t blen, const char* find, const char* replace);
    uint8_t HexChar2Nibble(char c);
	bool Ascii2HexChar(char c1, char c2, uint8_t& out);

    template<typename T>
    void Swap(T& a, T& b)
    {
        T tmp = a;
        a = b;
        b = tmp;
    }
        
    enum OutputMode
    {
        Hex,
        Dec
    };
    class NumberSize
    {
        friend class OutputStream;
    public:
        NumberSize(uint32_t size = 0, char padding = '0') :size(size), padding(padding){}
    private:
        uint32_t size;
        char padding;
    };
    
    class Endline
    {
    };

    const static Endline Endl;

    class OutputStream
	{
	protected:
        OutputStream();
	public:
        void EndlIndicator(const char* indicator);
		OutputStream& operator<<(const NumberSize& ns);
		OutputStream& operator<<(OutputMode outputMode);
#ifndef WIN32
		OutputStream& operator<<(int v);
#endif
        OutputStream& operator<<(Endline e);
        OutputStream& operator<<(bool v);
        OutputStream& operator<<(char v);
        OutputStream& operator<<(int8_t v);
		OutputStream& operator<<(uint8_t v);
		OutputStream& operator<<(int16_t v);
		OutputStream& operator<<(uint16_t v);
        OutputStream& operator<<(int32_t v);
        OutputStream& operator<<(uint32_t v);
        OutputStream& operator<<(int64_t v);
        OutputStream& operator<<(uint64_t v);
        OutputStream& operator<<(float v);
        OutputStream& operator<<(const char* txt);
    protected:
        OutputMode GetOutputMode() const { return mode; }
        NumberSize GetNumberSize() const { return numberSize; }
        virtual void ProcessOutput(char c) = 0;
    private:
		void AddDec(uint32_t v);
		void AddHex(uint32_t v);

        OutputMode mode;
        NumberSize numberSize;
        const char* endLineIndicator;
	};

	template<uint32_t S>
	class OutputStreamBuffered : public OutputStream
	{
	public:
		OutputStreamBuffered() 
            : OutputStream()
            , positionWr(0)
        {}
        uint32_t StrLen() const { return positionWr; }
        operator const char*()
        {
            buffer[positionWr] = 0;
            return buffer;
        }
    protected:
        void ProcessOutput(char c)
        {
            if (positionWr<(S - 1))
                buffer[positionWr++] = c;
        }

    private:
        char buffer[S];
        
        uint32_t positionWr;
    };

    class InputStream
    {
    protected:
        InputStream();
    public:
        bool Fail() const;
#ifndef WIN32
        InputStream& operator>>(int& v);
#endif
        InputStream& operator>>(bool& v);
        InputStream& operator>>(char& v);
        InputStream& operator>>(int8_t& v);
        InputStream& operator>>(uint8_t& v);
        InputStream& operator>>(int16_t& v);
        InputStream& operator>>(uint16_t& v);
        InputStream& operator>>(int32_t& v);
        InputStream& operator>>(uint32_t& v);
        InputStream& operator>>(int64_t& v);
        InputStream& operator>>(uint64_t& v);
        InputStream& operator>>(float& v);
    protected:
        virtual char GetInput() = 0;
    private:
        char Read();
        char Peek();
        int16_t peek = -1;
        bool outOfData = false;
    };

    class InputStreamString : public InputStream
    {
    public:
        InputStreamString(const char* input)
            : input(input)
        {}
    protected:
        char GetInput() override
        {
            return *input++;
        }
    private:
        const char* input;
    };
}
