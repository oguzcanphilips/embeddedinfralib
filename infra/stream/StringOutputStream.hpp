#ifndef INFRA_STRING_OUTPUT_STREAM_HPP
#define INFRA_STRING_OUTPUT_STREAM_HPP

#include "infra/stream/StreamManipulators.hpp"
#include "infra/stream/OutputStream.hpp"
#include "infra/util/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class StringOutputStreamWriter
        : public StreamWriter
    {
    public:
        explicit StringOutputStreamWriter(BoundedString& string);
        StringOutputStreamWriter(BoundedString& string, SoftFail);
        StringOutputStreamWriter(BoundedString& string, NoFail);

        template<class T>
            ReservedProxy<T> Reserve();

    private:
        virtual void Insert(ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;
        virtual std::size_t Available() const override;

        virtual const uint8_t* ConstructSaveMarker() const override;
        virtual std::size_t GetProcessedBytesSince(const uint8_t* marker) const override;
        virtual infra::ByteRange SaveState(const uint8_t* marker) override;
        virtual void RestoreState(infra::ByteRange range) override;

    private:
        BoundedString& string;
    };

    class StringOutputStream
        : public TextOutputStream::WithWriter<StringOutputStreamWriter>
    {
    public:
        template<std::size_t Max>
            using WithStorage = infra::WithStorage<TextOutputStream::WithWriter<StringOutputStreamWriter>, BoundedString::WithStorage<Max>>;

        using TextOutputStream::WithWriter<StringOutputStreamWriter>::WithWriter;
    };

    ////    Implementation    ////

    template<class T>
    ReservedProxy<T> StringOutputStreamWriter::Reserve()
    {
        ByteRange range(ReinterpretCastByteRange(MemoryRange<char>(string.end(), string.end() + sizeof(T))));
        std::size_t spaceLeft = string.max_size() - string.size();
        bool spaceOk = range.size() <= spaceLeft;
        ReportResult(spaceOk);
        if (!spaceOk)
            range.shrink_from_back_to(spaceLeft);

        string.append(range.size(), 0);

        return ReservedProxy<T>(range);
    }
}

#endif
