#ifndef INFRA_OUTPUT_STREAM_HPP
#define INFRA_OUTPUT_STREAM_HPP

#include "infra/stream/StreamManipulators.hpp"
#include "infra/util/BoundedString.hpp"
#include "infra/util/ByteRange.hpp"
#include "infra/util/Optional.hpp"
#include <cstdlib>
#include <string>
#include <type_traits>

namespace infra
{
    class TextOutputStream;

    class StreamWriter
    {
    public:
        StreamWriter() = default;
        explicit StreamWriter(SoftFail);
        explicit StreamWriter(NoFail);
        StreamWriter(const StreamWriter&) = delete;
        StreamWriter& operator=(const StreamWriter&) = delete;

    protected:
        ~StreamWriter();

    public:
        virtual void Insert(ConstByteRange range) = 0;
        virtual void Insert(uint8_t element) = 0;

        virtual const uint8_t* ConstructSaveMarker() const;
        virtual std::size_t GetProcessedBytesSince(const uint8_t* marker) const;
        virtual infra::ByteRange SaveState(const uint8_t* marker);
        virtual void RestoreState(infra::ByteRange range);

        void SetNoFail();
        bool HasFailed() const;
        void ReportResult(bool ok);

    private:
        enum class FailureMode
        {
            assertion,
            soft,
            no
        };

        FailureMode failureMode = FailureMode::assertion;
        bool failed = false;
        mutable bool checkedFail = true;
    };
    
    class StreamWriterDummy
      : public StreamWriter
    {
    public:
        virtual void Insert(ConstByteRange range);
        virtual void Insert(uint8_t element);
    };    

    class OutputStream
    {
    protected:
        explicit OutputStream(StreamWriter& writer);
        ~OutputStream() = default;

    public:
        void SetNoFail();
        bool Failed() const;
        const uint8_t* SaveMarker() const;
        std::size_t ProcessedBytesSince(const uint8_t* marker) const;

        StreamWriter& Writer();

    private:
        StreamWriter& writer;
    };

    class DataOutputStream
        : public OutputStream
    {
    public:
        explicit DataOutputStream(StreamWriter& writer);

        TextOutputStream operator<<(Text);

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
        TextOutputStream operator<<(Bin);
        TextOutputStream operator<<(Width width);
        DataOutputStream operator<<(Data);
        TextOutputStream& operator<<(Endl);

        TextOutputStream& operator<<(const char* zeroTerminatedString);
        TextOutputStream& operator<<(BoundedConstString string);
        TextOutputStream& operator<<(const std::string& string);
        TextOutputStream& operator<<(char c);
        TextOutputStream& operator<<(uint8_t v);
        TextOutputStream& operator<<(int32_t v);
        TextOutputStream& operator<<(uint32_t v);
        TextOutputStream& operator<<(int64_t v);
        TextOutputStream& operator<<(uint64_t v);
#ifndef _MSC_VER                                                                                        //TICS !POR#021
        TextOutputStream& operator<<(int v);
        TextOutputStream& operator<<(unsigned int v);
#endif
        TextOutputStream& operator<<(float v);

        template<class... Args>
            void Format(const char* format, Args&&... arguments);

    private:
        class FormatterBase
        {
        public:
            FormatterBase() = default;

        protected:
            FormatterBase(const FormatterBase& other) = default;
            FormatterBase& operator=(const FormatterBase& other) = default;
            ~FormatterBase() = default;

        public:
            virtual void Stream(TextOutputStream& stream) = 0;

        };

        template<class T>
        class Formatter
            : public FormatterBase
        {
        public:
            explicit Formatter(T value);
            Formatter(const Formatter& other) = default;
            Formatter& operator=(const Formatter& other) = default;

            virtual void Stream(TextOutputStream& stream) override;

        private:
            T value;
        };

        template<class T>
            Formatter<T> MakeFormatter(T&& argument);

    private:
        void OutputAsDecimal(uint64_t v);
        void OutputAsBinary(uint64_t v);
        void OutputAsHexadecimal(uint64_t v);

        template<class... Formatters>
            void FormatHelper(const char* format, Formatters&&... formatters);
        void FormatArgs(const char* format, infra::MemoryRange<FormatterBase*> formatters);

    private:
        enum class Radix
        {
            dec, bin, hex
        };

        Radix radix = Radix::dec;
        infra::Optional<Width> width;
    };
    
    class AsAsciiHelper
    {
    public:
        explicit AsAsciiHelper(infra::ConstByteRange data);

        friend infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const AsAsciiHelper& asAsciiHelper);

    private:
        infra::ConstByteRange data;
    };

    class AsHexHelper
    {
    public:
        explicit AsHexHelper(infra::ConstByteRange data);

        friend infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const AsHexHelper& asHexHelper);

    private:
        infra::ConstByteRange data;
    };

    class AsBase64Helper
    {
    public:
        explicit AsBase64Helper(infra::ConstByteRange data);

        friend infra::TextOutputStream& operator<<(infra::TextOutputStream& stream, const AsBase64Helper& asBase64Helper);

    private:
        infra::ConstByteRange data;
    };

    AsAsciiHelper AsAscii(infra::ConstByteRange data);
    AsHexHelper AsHex(infra::ConstByteRange data);
    AsBase64Helper AsBase64(infra::ConstByteRange data);

    template<class T>
    class ReservedProxy
    {
    public:
        explicit ReservedProxy(ByteRange range);

        ReservedProxy& operator=(const T& data);

    private:
        ByteRange range;
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

    template<class T>
    TextOutputStream::Formatter<T>::Formatter(T value)
        : value(value)
    {}

    template<class T>
    void TextOutputStream::Formatter<T>::Stream(TextOutputStream& stream)
    {
        stream << value;
    }

    template<class T>
    TextOutputStream::Formatter<T> TextOutputStream::MakeFormatter(T&& argument)
    {
        return Formatter<T>(std::forward<T>(argument));
    }

    template<class... Args>
    void TextOutputStream::Format(const char* format, Args&&... arguments)
    {
        FormatHelper(format, MakeFormatter(arguments)...);
    }

    template<class... Args>
    void TextOutputStream::FormatHelper(const char* format, Args&&... arguments)
    {
        std::array<FormatterBase*, sizeof...(Args)> formatters = { &arguments... };
        FormatArgs(format, formatters);
    }

    template<class T>
    ReservedProxy<T>::ReservedProxy(ByteRange range)
        : range(range)
    {}

    template<class T>
    ReservedProxy<T>& ReservedProxy<T>::operator=(const T& data)
    {
        if (range.size() == sizeof(data))
            Copy(infra::MakeByteRange(data), range);

        return *this;
    }
}

#endif
