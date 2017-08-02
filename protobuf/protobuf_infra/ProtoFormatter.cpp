#include "infra/stream/SavedMarkerStream.hpp"
#include "protobuf/protobuf_infra/ProtoFormatter.hpp"

namespace services
{
    ProtoLengthDelimitedFormatter::ProtoLengthDelimitedFormatter(ProtoFormatter& formatter, uint32_t fieldNumber)
        : formatter(formatter)
    {
        formatter.PutVarUint32((fieldNumber << 3) | 2);
        marker = formatter.output.SaveMarker();
    }

    ProtoLengthDelimitedFormatter::ProtoLengthDelimitedFormatter(ProtoLengthDelimitedFormatter&& other)
        : formatter(other.formatter)
        , marker(other.marker)
    {
        other.marker = nullptr;
    }

    ProtoLengthDelimitedFormatter::~ProtoLengthDelimitedFormatter()
    {
        if (marker != nullptr)
        {
            uint32_t size = formatter.output.ProcessedBytesSince(marker);
            infra::SavedMarkerDataStream savedStream(formatter.output, marker);
            ProtoFormatter savedFormatter(savedStream);
            savedFormatter.PutVarUint32(size);
        }
    }

    ProtoFormatter::ProtoFormatter(infra::DataOutputStream output)
        : output(output.Writer())
    {}

    void ProtoFormatter::PutVarUint32(uint32_t value)
    {
        do
        {
            if (value > 127)
                output << static_cast<uint8_t>(value & 0x7f | 0x80);
            else
                output << static_cast<uint8_t>(value);

            value >>= 7;
        } while (value != 0);
    }

    void ProtoFormatter::PutUint32(uint32_t value, uint32_t fieldNumber)
    {
        PutVarUint32((fieldNumber << 3) | 0);
        PutVarUint32(value);
    }

    void ProtoFormatter::PutLengthDelimited(infra::ConstByteRange range, uint32_t fieldNumber)
    {
        PutVarUint32((fieldNumber << 3) | 2);
        PutVarUint32(range.size());
        output << range;
    }

    void ProtoFormatter::PutString(infra::BoundedConstString string, uint32_t fieldNumber)
    {
        PutLengthDelimited(infra::StringAsByteRange(string), fieldNumber);
    }

    ProtoLengthDelimitedFormatter ProtoFormatter::LengthDelimitedFormatter(uint32_t fieldNumber)
    {
        return ProtoLengthDelimitedFormatter(*this, fieldNumber);
    }
}
