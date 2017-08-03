#ifndef PROTOBUF_PROTO_FORMATTER_HPP
#define PROTOBUF_PROTO_FORMATTER_HPP

#include "infra/stream/OutputStream.hpp"

namespace services
{
    class ProtoFormatter;

    class ProtoLengthDelimitedFormatter
    {
    public:
        ProtoLengthDelimitedFormatter(ProtoFormatter& formatter, uint32_t fieldNumber);
        ProtoLengthDelimitedFormatter(const ProtoLengthDelimitedFormatter& other) = delete;
        ProtoLengthDelimitedFormatter(ProtoLengthDelimitedFormatter&& other);
        ProtoLengthDelimitedFormatter& operator=(const ProtoLengthDelimitedFormatter& other) = delete;
        ~ProtoLengthDelimitedFormatter();

    private:
        ProtoFormatter& formatter;
        const uint8_t* marker;
    };

    class ProtoFormatter
    {
    public:
        explicit ProtoFormatter(infra::DataOutputStream output);

        void PutVarUint32(uint32_t value);
        void PutUint32(uint32_t value, uint32_t fieldNumber);
        void PutLengthDelimited(infra::ConstByteRange range, uint32_t fieldNumber);
        void PutString(infra::BoundedConstString string, uint32_t fieldNumber);
        ProtoLengthDelimitedFormatter LengthDelimitedFormatter(uint32_t fieldNumber);

    private:
        friend class ProtoLengthDelimitedFormatter;
        infra::DataOutputStream output;
    };
}

#endif
