#ifndef INFRA_IO_OUTPUT_STREAM_HPP
#define INFRA_IO_OUTPUT_STREAM_HPP

#include "infra/stream/StreamManipulators.hpp"
#include "infra/stream/OutputStream.hpp"
#include "infra/util/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class IoOutputStreamWriter
        : public StreamWriter
    {
    private:
        virtual void Insert(ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;
        virtual std::size_t Available() const override;

        virtual const uint8_t* ConstructSaveMarker() const override;
        virtual std::size_t GetProcessedBytesSince(const uint8_t* marker) const override;
        virtual infra::ByteRange SaveState(const uint8_t* marker) override;
        virtual void RestoreState(infra::ByteRange range) override;
    };

    class IoOutputStream
        : public TextOutputStream::WithWriter<IoOutputStreamWriter>
    {
    public:
        template<std::size_t Max>
            using WithStorage = infra::WithStorage<TextOutputStream::WithWriter<IoOutputStreamWriter>, BoundedString::WithStorage<Max>>;

        using TextOutputStream::WithWriter<IoOutputStreamWriter>::WithWriter;
    };
}

#endif
