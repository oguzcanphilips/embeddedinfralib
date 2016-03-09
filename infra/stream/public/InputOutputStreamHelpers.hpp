#ifndef INFRA_INPUT_OUTPUT_STREAM_HELPERS_HPP
#define INFRA_INPUT_OUTPUT_STREAM_HELPERS_HPP

#include "infra/stream/public/InputOutputStream.hpp"
#include "infra/util/public/Optional.hpp"
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace infra
{
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
        TextInputStreamHelper<T> operator>>(DataInputStreamHelper<T>& stream, Text);
    template<class T>
        DataInputStreamHelper<T> operator>>(TextInputStreamHelper<T>& textStream, Data);
    template<class T>
        TextInputStreamHelper<T> operator>>(TextInputStreamHelper<T>& textStream, Hex);
    template<class T>
        TextInputStreamHelper<T> operator>>(TextInputStreamHelper<T>& textStream, Width);

    ////    Implementation    ////

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
}

#endif
