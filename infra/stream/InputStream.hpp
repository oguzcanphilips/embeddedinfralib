#ifndef INFRA_INPUT_STREAM_HPP
#define INFRA_INPUT_STREAM_HPP

#include "infra/stream/StreamErrorPolicy.hpp"
#include "infra/stream/StreamManipulators.hpp"
#include "infra/util/BoundedString.hpp"
#include "infra/util/ByteRange.hpp"
#include "infra/util/Optional.hpp"
#include <cstdlib>
#include <type_traits>

namespace infra
{
    class TextInputStream;

    class StreamReader
    {
    protected:
        StreamReader() = default;
        StreamReader(SoftFail);
        StreamReader(const StreamReader& other) = delete;
        StreamReader& operator=(const StreamReader& other) = delete;
        ~StreamReader();

    public:
        virtual void Extract(ByteRange range) = 0;
        virtual uint8_t ExtractOne() = 0;
        virtual uint8_t Peek() = 0;
        virtual ConstByteRange ExtractContiguousRange(std::size_t max) = 0;

        virtual bool Empty() const = 0;
        virtual std::size_t Available() const = 0;

        virtual bool Failed() const;
        virtual void ReportResult(bool ok);

        virtual void SetSoftFail();

    private:
        bool softFail = false;
        bool failed = false;
        mutable bool checkedFail = true;
    };

    class InputStream
    {
    protected:
        explicit InputStream(StreamReader& reader);
        InputStream(StreamReader& reader, infra::SoftFail);
        ~InputStream() = default;

    public:
        bool Empty() const;
        std::size_t Available() const;
        ConstByteRange ContiguousRange(std::size_t max = std::numeric_limits<std::size_t>::max());
        bool Failed() const;

        StreamReader& Reader();

    private:
        StreamReader& reader;
    };

    class DataInputStream
        : public InputStream
    {
    public:
        template<class Reader>
            class WithReader;

        explicit DataInputStream(StreamReader& reader);
        DataInputStream(StreamReader& reader, SoftFail);

        TextInputStream operator>>(Text);
        DataInputStream& operator>>(ByteRange range);
        template<class Data>
            DataInputStream& operator>>(Data& data);
    };

    class TextInputStream
        : public InputStream
    {
    public:
        template<class Reader>
            class WithReader;

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

    template<class TheReader>
    class DataInputStream::WithReader
        : private detail::StorageHolder<TheReader, WithReader<TheReader>>
        , public DataInputStream
    {
    public:
        template<class... Args>
            WithReader(Args&&... args);

        TheReader& Reader();
    };

    template<class TheReader>
    class TextInputStream::WithReader
        : private detail::StorageHolder<TheReader, WithReader<TheReader>>
        , public TextInputStream
    {
    public:
        template<class... Args>
            WithReader(Args&&... args);

        TheReader& Reader();
    };

    ////    Implementation    ////

    template<class Data>
    DataInputStream& DataInputStream::operator>>(Data& data)
    {
        MemoryRange<typename std::remove_const<uint8_t>::type> dataRange(ReinterpretCastMemoryRange<typename std::remove_const<uint8_t>::type>(MakeRange(&data, &data + 1)));
        Reader().Extract(dataRange);
        return *this;
    }

    template<class TheReader>
    template<class... Args>
    DataInputStream::WithReader<TheReader>::WithReader(Args&&... args)
        : detail::StorageHolder<TheReader, WithReader<TheReader>>(std::forward<Args>(args)...)
        , DataInputStream(this->storage)
    {}

    template<class TheReader>
    TheReader& DataInputStream::WithReader<TheReader>::Reader()
    {
        return this->storage;
    }

    template<class TheReader>
    template<class... Args>
    TextInputStream::WithReader<TheReader>::WithReader(Args&&... args)
        : detail::StorageHolder<TheReader, WithReader<TheReader>>(std::forward<Args>(args)...)
        , TextInputStream(this->storage)
    {}

    template<class TheReader>
    TheReader& TextInputStream::WithReader<TheReader>::Reader()
    {
        return this->storage;
    }
}

#endif
