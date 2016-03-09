#ifndef INFRA_OUTPUT_STREAM_HPP
#define INFRA_OUTPUT_STREAM_HPP

#include "infra/stream/public/StreamManipulators.hpp"
#include "infra/util/public/ByteRange.hpp"
#include "infra/util/public/Optional.hpp"
#include <cstdlib>
#include <type_traits>

namespace infra
{
    class TextOutputStream;

    class OutputStreamWriter
    {
    public:
        virtual void Insert(ConstByteRange range) = 0;
        virtual void Insert(uint8_t element) = 0;
        virtual void Forward(std::size_t amount) = 0;

    protected:
        ~OutputStreamWriter() = default;
    };

    class DataOutputStream
    {
    public:
        DataOutputStream(OutputStreamWriter& writer);

        TextOutputStream operator<<(Text);

        template<class Data>
        DataOutputStream& operator<<(const Data& data);
        template<class Data>
        DataOutputStream& operator<<(MemoryRange<Data> data);

    private:
        OutputStreamWriter& writer;
    };

    class TextOutputStream
    {
    public:
        explicit TextOutputStream(OutputStreamWriter& stream);

        TextOutputStream operator<<(Hex);
        TextOutputStream operator<<(Width width);
        DataOutputStream operator<<(Data);

        TextOutputStream& operator<<(const char* zeroTerminatedString);
        TextOutputStream& operator<<(char c);
        TextOutputStream& operator<<(uint8_t v);
        TextOutputStream& operator<<(int32_t v);
        TextOutputStream& operator<<(uint32_t v);

    private:
        void OutputAsDecimal(uint32_t v);
        void OutputAsHex(uint32_t v);

    private:
        OutputStreamWriter& writer;
        bool decimal = true;
        infra::Optional<std::size_t> width;
    };

    ////    Implementation    ////

    template<class Data>
    DataOutputStream& DataOutputStream::operator<<(const Data& data)
    {
        ConstByteRange dataRange(ReinterpretCastByteRange(MakeRange(&data, &data + 1)));
        writer.Insert(dataRange);
        return *this;
    }

    template<class Data>
    DataOutputStream& DataOutputStream::operator<<(MemoryRange<Data> data)
    {
        ConstByteRange dataRange(ReinterpretCastByteRange(data));
        writer.Insert(dataRange);
        return *this;
    }
}

#endif
