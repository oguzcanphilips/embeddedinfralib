#ifndef INFRA_OUTPUT_STREAM_HPP
#define INFRA_OUTPUT_STREAM_HPP

#include "infra/stream/public/StreamManipulators.hpp"
#include "infra/util/public/BoundedString.hpp"
#include "infra/util/public/ByteRange.hpp"
#include "infra/util/public/Optional.hpp"
#include <cstdlib>
#include <type_traits>

namespace infra
{
    class TextOutputStream;

    class StreamWriter
    {
    public:
        StreamWriter();
        StreamWriter(SoftFail);
        ~StreamWriter();

        virtual void Insert(ConstByteRange range) = 0;
        virtual void Insert(uint8_t element) = 0;
        virtual void Forward(std::size_t amount) = 0;

        bool Failed() const;
        void ReportResult(bool ok);

    private:
        bool softFail = false;
        bool failed = false;
        mutable bool checkedFail = true;
    };

    class OutputStream
    {
    public:
        bool HasFailed() const;

    protected:
        OutputStream(StreamWriter& writer);
        StreamWriter& Writer();

    private:
        StreamWriter& writer;
    };

    class DataOutputStream
        : public OutputStream
    {
    public:
        DataOutputStream(StreamWriter& writer);

        TextOutputStream operator<<(Text);
        DataOutputStream& operator<<(ForwardStream forward);

        template<class Data>
            DataOutputStream& operator<<(const Data& data);
        template<class Data>
            DataOutputStream& operator<<(MemoryRange<Data> data);
    };

    class TextOutputStream
        : public OutputStream
    {
    public:
        explicit TextOutputStream(StreamWriter& stream);

        TextOutputStream operator<<(Hex);
        TextOutputStream operator<<(Width width);
        DataOutputStream operator<<(Data);
        TextOutputStream& operator<<(Endl);

        TextOutputStream& operator<<(const char* zeroTerminatedString);
        TextOutputStream& operator<<(BoundedConstString string);
        TextOutputStream& operator<<(char c);
        TextOutputStream& operator<<(uint8_t v);
        TextOutputStream& operator<<(int32_t v);
        TextOutputStream& operator<<(uint32_t v);
        TextOutputStream& operator<<(float v);

    private:
        void OutputAsDecimal(uint32_t v);
        void OutputAsHex(uint32_t v);

    private:
        bool decimal = true;
        infra::Optional<Width> width;
    };

    ////    Implementation    ////

    template<class Data>
    DataOutputStream& DataOutputStream::operator<<(const Data& data)
    {
        ConstByteRange dataRange(ReinterpretCastByteRange(MakeRange(&data, &data + 1)));
        Writer().Insert(dataRange);
        return *this;
    }

    template<class Data>
    DataOutputStream& DataOutputStream::operator<<(MemoryRange<Data> data)
    {
        ConstByteRange dataRange(ReinterpretCastByteRange(data));
        Writer().Insert(dataRange);
        return *this;
    }
}

#endif
