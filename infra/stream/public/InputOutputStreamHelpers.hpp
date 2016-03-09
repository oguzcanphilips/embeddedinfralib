#ifndef INFRA_INPUT_OUTPUT_STREAM_HELPERS_HPP
#define INFRA_INPUT_OUTPUT_STREAM_HELPERS_HPP

#include "infra/stream/public/InputOutputStream.hpp"
#include "infra/util/public/Optional.hpp"
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace infra
{
    struct Text {};
    struct Hex {};
    struct Data {};

    const Text text;
    const Hex hex;
    const Data data;

    struct Width
    {
        explicit Width(std::size_t width);

        std::size_t width;
    };

    template<class T>
    class DataInputStreamHelper
        : public IndirectInputStream<T>
    {
    public:
        DataInputStreamHelper(InputStream<T>& stream);

        template<class Data>
            DataInputStreamHelper& operator>>(Data& data);
        template<class Data>
            DataInputStreamHelper& operator>>(Data&& data);
        template<class Data>
            DataInputStreamHelper& operator>>(MemoryRange<Data> data);
    };

    template<class T>
    class DataOutputStreamHelper
        : public IndirectOutputStream<T>
    {
    public:
        DataOutputStreamHelper(OutputStream<T>& stream);

        template<class Data>
            DataOutputStreamHelper& operator<<(const Data& data);
        template<class Data>
            DataOutputStreamHelper& operator<<(MemoryRange<Data> data);
    };

    template<class T>
    class TextInputStreamHelper
        : public IndirectInputStream<T>
    {
    public:
        explicit TextInputStreamHelper(InputStream<T>& stream);
        TextInputStreamHelper(InputStream<T>& stream, SoftFail);
        TextInputStreamHelper(TextInputStreamHelper<T>& stream, Hex);
        TextInputStreamHelper(TextInputStreamHelper<T>& stream, Width width);

        TextInputStreamHelper& operator>>(char& c);
        TextInputStreamHelper& operator>>(uint8_t& v);
        TextInputStreamHelper& operator>>(uint16_t& v);
        TextInputStreamHelper& operator>>(uint32_t& v);
        TextInputStreamHelper& operator>>(const char* literal);

    private:
        void InputAsDecimal(uint32_t& v);
        void InputAsHex(uint32_t& v);

    private:
        bool decimal = true;
        infra::Optional<std::size_t> width;
    };

    template<class T>
    class TextOutputStreamHelper
        : public IndirectOutputStream<T>
    {
    public:
        explicit TextOutputStreamHelper(OutputStream<T>& stream);
        TextOutputStreamHelper(TextOutputStreamHelper<T>& stream, Hex);
        TextOutputStreamHelper(TextOutputStreamHelper<T>& stream, Width width);

        TextOutputStreamHelper& operator<<(const char* zeroTerminatedString);
        TextOutputStreamHelper& operator<<(char c);
        TextOutputStreamHelper& operator<<(uint8_t v);
        TextOutputStreamHelper& operator<<(uint32_t v);
        TextOutputStreamHelper& operator<<(int32_t v);

    private:
        void OutputAsDecimal(uint32_t v);
        void OutputAsHex(uint32_t v);

    private:
        bool decimal = true;
        infra::Optional<std::size_t> width;
    };

    template<class T>
        TextInputStreamHelper<T> operator>>(DataInputStreamHelper<T>& stream, Text);
    template<class T>
        DataInputStreamHelper<T> operator>>(TextInputStreamHelper<T>& textStream, Data);
    template<class T>
        TextInputStreamHelper<T> operator>>(TextInputStreamHelper<T>& textStream, Hex);
    template<class T>
        TextInputStreamHelper<T> operator>>(TextInputStreamHelper<T>& textStream, Width);

    template<class T>
        TextOutputStreamHelper<T> operator<<(DataOutputStreamHelper<T>& stream, Text);
    template<class T>
        DataOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>& textStream, Data);
    template<class T>
        TextOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>& textStream, Hex);
    template<class T>
        TextOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>& textStream, Width);
    template<class T>
        TextOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>&& textStream, Hex);
    template<class T>
        TextOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>&& textStream, Width);

    ////    Implementation    ////

    inline Width::Width(std::size_t width)
        : width(width)
    {}

    template<class T>
    DataInputStreamHelper<T>::DataInputStreamHelper(InputStream<T>& stream)
        : IndirectInputStream<T>(stream)
    {}

    template<class T>
    template<class Data>
    DataInputStreamHelper<T>& DataInputStreamHelper<T>::operator>>(Data& data)
    {
        MemoryRange<typename std::remove_const<T>::type> dataRange(ReinterpretCastMemoryRange<typename std::remove_const<T>::type>(MakeRange(&data, &data + 1)));
        this->stream.Extract(dataRange);
        return *this;
    }

    template<class T>
    template<class Data>
    DataInputStreamHelper<T>& DataInputStreamHelper<T>::operator>>(Data&& data)
    {
        MemoryRange<typename std::remove_const<T>::type> dataRange(ReinterpretCastMemoryRange<typename std::remove_const<T>::type>(MakeRange(&data, &data + 1)));
        this->stream.Extract(dataRange);
        return *this;
    }

    template<class T>
    template<class Data>
    DataInputStreamHelper<T>& DataInputStreamHelper<T>::operator>>(MemoryRange<Data> data)
    {
        MemoryRange<typename std::remove_const<T>::type> dataRange(ReinterpretCastMemoryRange<typename std::remove_const<T>::type>(data));
        this->stream.Extract(dataRange);
        return *this;
    }

    template<class T>
    DataOutputStreamHelper<T>::DataOutputStreamHelper(OutputStream<T>& stream)
        : IndirectOutputStream<T>(stream)
    {}

    template<class T>
    template<class Data>
    DataOutputStreamHelper<T>& DataOutputStreamHelper<T>::operator<<(const Data& data)
    {
        MemoryRange<const T> dataRange(ReinterpretCastMemoryRange<const T>(MakeRange(&data, &data + 1)));
        this->stream.Insert(dataRange);
        return *this;
    }

    template<class T>
    template<class Data>
    DataOutputStreamHelper<T>& DataOutputStreamHelper<T>::operator<<(MemoryRange<Data> data)
    {
        MemoryRange<const T> dataRange(ReinterpretCastMemoryRange<const T>(data));
        this->stream.Insert(dataRange);
        return *this;
    }

    template<class T>
    TextInputStreamHelper<T>::TextInputStreamHelper(InputStream<T>& stream)
        : IndirectInputStream<T>(stream)
    {}

    template<class T>
    TextInputStreamHelper<T>::TextInputStreamHelper(InputStream<T>& stream, SoftFail)
        : IndirectInputStream<T>(stream, softFail)
    {}

    template<class T>
    TextInputStreamHelper<T>::TextInputStreamHelper(TextInputStreamHelper<T>& stream, Hex)
        : IndirectInputStream<T>(static_cast<InputStream<T>&>(stream))
        , decimal(false)
        , width(stream.width)
    {}

    template<class T>
    TextInputStreamHelper<T>::TextInputStreamHelper(TextInputStreamHelper<T>& stream, Width width)
        : IndirectInputStream<T>(static_cast<InputStream<T>&>(stream))
        , decimal(stream.decimal)
        , width(infra::inPlace, width.width)
    {}

    template<class T>
    TextInputStreamHelper<T>& TextInputStreamHelper<T>::operator>>(char& c)
    {
        this->stream.Extract(c);
        return *this;
    }

    template<class T>
    TextInputStreamHelper<T>& TextInputStreamHelper<T>::operator>>(uint8_t& v)
    {
        uint32_t extracted(0);
        if (decimal)
            InputAsDecimal(extracted);
        else
            InputAsHex(extracted);
        v = static_cast<uint8_t>(extracted);
        return *this;
    }

    template<class T>
    TextInputStreamHelper<T>& TextInputStreamHelper<T>::operator>>(uint16_t& v)
    {
        uint32_t extracted(0);
        if (decimal)
            InputAsDecimal(extracted);
        else
            InputAsHex(extracted);
        v = static_cast<uint16_t>(extracted);
        return *this;
    }

    template<class T>
    TextInputStreamHelper<T>& TextInputStreamHelper<T>::operator>>(uint32_t& v)
    {
        if (decimal)
            InputAsDecimal(v);
        else
            InputAsHex(v);
        return *this;
    }

    template<class T>
    TextInputStreamHelper<T>& TextInputStreamHelper<T>::operator>>(const char* literal)
    {
        while (*literal)
        {
            char c;
            this->stream.Extract(c);

            this->stream.ReportFailureCheck(c != *literal);
            ++literal;
        }

        return *this;
    }

    template<class T>
    void TextInputStreamHelper<T>::InputAsDecimal(uint32_t& v)
    {
        v = 0;

        for (std::size_t i = 0; (i != width.ValueOr(std::numeric_limits<std::size_t>::max()) && !this->stream.Empty()) || i == 0; ++i)
        {
            char c;
            this->stream.Peek(c);

            if (c >= '0' && c <= '9')
                v = v * 10 + c - '0';
            else
            {
                this->stream.ReportFailureCheck(i == 0);
                break;
            }

            this->stream.Extract(c);
        }
    }

    template<class T>
    void TextInputStreamHelper<T>::InputAsHex(uint32_t& v)
    {
        static const char hexChars[] = "0123456789abcdef";

        v = 0;

        for (std::size_t i = 0; (i != width.ValueOr(std::numeric_limits<std::size_t>::max()) && !this->stream.Empty()) || i == 0; ++i)
        {
            char c;
            this->stream.Peek(c);

            if (c >= '0' && c <= '9')
                v = (v << 4) + c - '0';
            else if (c >= 'a' && c <= 'f')
                v = (v << 4) + c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                v = (v << 4) + c - 'A' + 10;
            else
            {
                this->stream.ReportFailureCheck(i == 0);
                break;
            }

            this->stream.Extract(c);
        }
    }

    template<class T>
    TextOutputStreamHelper<T>::TextOutputStreamHelper(OutputStream<T>& stream)
        : IndirectOutputStream<T>(stream)
    {}

    template<class T>
    TextOutputStreamHelper<T>::TextOutputStreamHelper(TextOutputStreamHelper<T>& stream, Hex)
        : IndirectOutputStream<T>(static_cast<OutputStream<T>&>(stream))
        , decimal(false)
        , width(stream.width)
    {}

    template<class T>
    TextOutputStreamHelper<T>::TextOutputStreamHelper(TextOutputStreamHelper<T>& stream, Width width)
        : IndirectOutputStream<T>(static_cast<OutputStream<T>&>(stream))
        , decimal(stream.decimal)
        , width(infra::inPlace, width.width)
    {}

    template<class T>
    TextOutputStreamHelper<T>& TextOutputStreamHelper<T>::operator<<(const char* zeroTerminatedString)
    {
        this->stream.Insert(ReinterpretCastMemoryRange<const T>(MakeRange(zeroTerminatedString, zeroTerminatedString + std::strlen(zeroTerminatedString))));

        return *this;
    }

    template<class T>
    TextOutputStreamHelper<T>& TextOutputStreamHelper<T>::operator<<(char c)
    {
        this->stream.Insert(c);

        return *this;
    }

    template<class T>
    TextOutputStreamHelper<T>& TextOutputStreamHelper<T>::operator<<(uint8_t v)
    {
        if (decimal)
            OutputAsDecimal(v);
        else
            OutputAsHex(v);

        return *this;
    }

    template<class T>
    TextOutputStreamHelper<T>& TextOutputStreamHelper<T>::operator<<(uint32_t v)
    {
        if (decimal)
            OutputAsDecimal(v);
        else
            OutputAsHex(v);

        return *this;
    }

    template<class T>
    TextOutputStreamHelper<T>& TextOutputStreamHelper<T>::operator<<(int32_t v)
    {
        if (v < 0)
            this->stream.Insert('-');
        if (decimal)
            OutputAsDecimal(std::abs(v));
        else
            OutputAsHex(std::abs(v));

        return *this;
    }

    template<class T>
    void TextOutputStreamHelper<T>::OutputAsDecimal(uint32_t v)
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
                this->stream.Insert('0');

        while (mask)
        {
            this->stream.Insert(static_cast<char>(((v / mask) % 10) + '0'));
            mask /= 10;
        }
    }

    template<class T>
    void TextOutputStreamHelper<T>::OutputAsHex(uint32_t v)
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
                this->stream.Insert('0');

        while (mask)
        {
            this->stream.Insert(static_cast<char>(hexChars[(v / mask) % 16]));
            mask /= 16;
        }
    }

    template<class T>
    TextInputStreamHelper<T> operator>>(DataInputStreamHelper<T>& stream, Text)
    {
        return TextInputStreamHelper<T>(static_cast<InputStream<T>&>(stream));
    }

    template<class T>
    DataInputStreamHelper<T> operator>>(TextInputStreamHelper<T>& textStream, Data)
    {
        return DataInputStreamHelper<T>(static_cast<InputStream<T>&>(textStream));
    }

    template<class T>
    TextInputStreamHelper<T> operator>>(TextInputStreamHelper<T>& textStream, Hex)
    {
        return TextInputStreamHelper<T>(textStream, hex);
    }

    template<class T>
    TextInputStreamHelper<T> operator>>(TextInputStreamHelper<T>& textStream, Width width)
    {
        return TextInputStreamHelper<T>(textStream, width);
    }

    template<class T>
    TextOutputStreamHelper<T> operator<<(DataOutputStreamHelper<T>& stream, Text)
    {
        return TextOutputStreamHelper<T>(static_cast<OutputStream<T>&>(stream));
    }

    template<class T>
    DataOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>& textStream, Data)
    {
        return DataOutputStreamHelper<T>(static_cast<OutputStream<T>&>(textStream));
    }

    template<class T>
    TextOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>& textStream, Hex)
    {
        return TextOutputStreamHelper<T>(textStream, hex);
    }

    template<class T>
    TextOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>& textStream, Width width)
    {
        return TextOutputStreamHelper<T>(textStream, width);
    }

    template<class T>
    TextOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>&& textStream, Hex)
    {
        return TextOutputStreamHelper<T>(textStream, hex);
    }

    template<class T>
    TextOutputStreamHelper<T> operator<<(TextOutputStreamHelper<T>&& textStream, Width width)
    {
        return TextOutputStreamHelper<T>(textStream, width);
    }
}

#endif
