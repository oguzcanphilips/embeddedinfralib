#ifndef INFRA_INPUT_STREAM_HPP
#define INFRA_INPUT_STREAM_HPP

#include "infra/stream/public/StreamManipulators.hpp"
#include "infra/util/public/BoundedString.hpp"
#include "infra/util/public/ByteRange.hpp"
#include "infra/util/public/Optional.hpp"
#include <cstdlib>
#include <type_traits>

namespace infra
{
    class TextInputStream;

    class StreamReader
    {
    public:
        StreamReader();
        explicit StreamReader(SoftFail);
        ~StreamReader();

        virtual void Extract(ByteRange range) = 0;
        virtual uint8_t ExtractOne() = 0;
        virtual uint8_t Peek() = 0;
        virtual void Forward(std::size_t amount) = 0;
        virtual bool Empty() const = 0;

        bool Failed() const;
        void ReportResult(bool ok);

    private:
        bool softFail = false;
        bool failed = false;
        mutable bool checkedFail = true;
    };

    class InputStream
    {
    public:
        bool IsEmpty() const;
        bool HasFailed() const;

    protected:
        explicit InputStream(StreamReader& reader);
        ~InputStream() = default;

        StreamReader& Reader();

    private:
        StreamReader& reader;
    };

    class DataInputStream
        : public InputStream
    {
    public:
        explicit DataInputStream(StreamReader& reader);

        TextInputStream operator>>(Text);
        DataInputStream& operator>>(ForwardStream forward);
        DataInputStream& operator>>(ByteRange range);
        template<class Data>
            DataInputStream& operator>>(Data& data);
    };
    
    class TextInputStream
        : public InputStream
    {
    public:
        explicit TextInputStream(StreamReader& reader);
        TextInputStream(StreamReader& reader, SoftFail);

        DataInputStream operator>>(Data);
        TextInputStream operator>>(Hex);
        TextInputStream operator>>(Width width);

        TextInputStream& operator>>(MemoryRange<char> text);
        TextInputStream& operator>>(char& c);
        TextInputStream& operator>>(int8_t& v);
        TextInputStream& operator>>(int16_t& v);
        TextInputStream& operator>>(int32_t& v);
        TextInputStream& operator>>(uint8_t& v);
        TextInputStream& operator>>(uint16_t& v);
        TextInputStream& operator>>(uint32_t& v);
        TextInputStream& operator>>(float& v);
        TextInputStream& operator>>(BoundedString& v);
        TextInputStream& operator>>(const char* literal);

    private:
        void SkipSpaces();
        void Read(int32_t& v);
        void Read(uint32_t& v);
        void ReadAsDecimal(int32_t& v);
        void ReadAsDecimal(uint32_t& v);
        void ReadAsHex(int32_t& v);
        void ReadAsHex(uint32_t& v);

        bool isDecimal = true;
        infra::Optional<std::size_t> width;
    };

    template<class Data>
    DataInputStream& DataInputStream::operator>>(Data& data)
    {
        MemoryRange<typename std::remove_const<uint8_t>::type> dataRange(ReinterpretCastMemoryRange<typename std::remove_const<uint8_t>::type>(MakeRange(&data, &data + 1)));
        Reader().Extract(dataRange);
        return *this;
    }
}

#endif
