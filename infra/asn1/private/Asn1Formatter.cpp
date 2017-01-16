#include "infra/asn1/public/Asn1Formatter.hpp"

namespace
{
    void AddLength(infra::DataOutputStream& stream, uint32_t length)
    {
        if (length > 0xFF)
            stream << uint8_t(0x82) << uint8_t((length & 0xFF00) >> 8) << uint8_t(length & 0xFF);
        else if (length > 0x7F)
            stream << uint8_t(0x81) << uint8_t(length & 0xFF);
        else
            stream << uint8_t(length & 0xFF);
    }
}

namespace infra
{
    Asn1Formatter::Asn1Formatter(infra::DataOutputStream& stream)
        : stream(stream)
    {}

    void Asn1Formatter::Add(uint8_t value)
    {
        AddTag(Type::Integer, sizeof(uint8_t), value);
    }

    void Asn1Formatter::Add(uint32_t value)
    {
        value = (value << 16) | (value >> 16);
        value = ((value & 0x00ff00ff) << 8) | ((value & 0xff00ff00) >> 8);

        AddTag(Type::Integer, sizeof(uint32_t), value);
    }

    void Asn1Formatter::Add(int32_t value)
    {
        value = (value << 16) | (value >> 16);
        value = ((value & 0x00ff00ff) << 8) | ((value & 0xff00ff00) >> 8);

        AddTag(Type::Integer, sizeof(int32_t), value);
    }

    void Asn1Formatter::AddBigNumber(infra::ConstByteRange number)
    {
        AddTag(Type::Integer, number.size(), number);
    }

    void Asn1Formatter::AddObjectId(infra::ConstByteRange oid)
    {
        AddTag(Type::Oid, oid.size(), oid);
    }

    void Asn1Formatter::AddBitString(infra::ConstByteRange string)
    {
        AddTag(Type::BitString, string.size(), string);
    }

    void Asn1Formatter::AddPrintableString(infra::ConstByteRange string)
    {
        AddTag(Type::PrintableString, string.size(), string);
    }

    void Asn1Formatter::AddUtcTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
    {
        const uint8_t utcTimeSize = 13; // "YYMMDDhhmmssZ"
        AddTag(Type::UtcTime, utcTimeSize);

        year -= 1900;
        year -= 100 * (year >= 50);

        stream << infra::text << infra::Width(2, '0') << uint8_t(year);
        stream << infra::text << infra::Width(2, '0') << month;
        stream << infra::text << infra::Width(2, '0') << day;
        stream << infra::text << infra::Width(2, '0') << hour;
        stream << infra::text << infra::Width(2, '0') << min;
        stream << infra::text << infra::Width(2, '0') << sec;
        stream << infra::text << 'Z';
    }

    void Asn1Formatter::AddNull()
    {
        stream << uint8_t(0x00);
    }

    Asn1ContainerFormatter Asn1Formatter::StartSequence()
    {
        stream << static_cast<uint8_t>(Type::Constructed | Type::Sequence);

        return Asn1ContainerFormatter(stream, stream.SaveMarker());
    }

    Asn1ContainerFormatter Asn1Formatter::StartSet()
    {
        stream << static_cast<uint8_t>(Type::Constructed | Type::Set);

        return Asn1ContainerFormatter(stream, stream.SaveMarker());
    }

    Asn1ContainerFormatter Asn1Formatter::StartContextSpecific(uint8_t context)
    {
        stream << static_cast<uint8_t>(Type::Constructed | Type::ContextSpecific | context);

        return Asn1ContainerFormatter(stream, stream.SaveMarker());
    }

    Asn1ContainerFormatter Asn1Formatter::StartBitString()
    {
        stream << static_cast<uint8_t>(Type::BitString);

        return Asn1ContainerFormatter(stream, stream.SaveMarker());
    }

    bool Asn1Formatter::HasFailed() const
    {
        return stream.HasFailed();
    }

    infra::DataOutputStream& Asn1Formatter::Stream()
    {
        return stream;
    }

    void Asn1Formatter::AddTag(uint8_t type, uint32_t length)
    {
        stream << type;
        AddLength(stream, length);
    }

    Asn1ContainerFormatter::Asn1ContainerFormatter(infra::DataOutputStream& stream, const uint8_t* sizeMarker)
        : Asn1Formatter(stream)
        , sizeMarker(sizeMarker)
    {}

    Asn1ContainerFormatter::Asn1ContainerFormatter(Asn1ContainerFormatter&& other)
        : Asn1Formatter(Stream())
        , sizeMarker(other.sizeMarker)
    {
        other.sizeMarker = nullptr;
    }

    Asn1ContainerFormatter& Asn1ContainerFormatter::operator=(Asn1ContainerFormatter&& other)
    {
        sizeMarker = other.sizeMarker;
        other.sizeMarker = nullptr;

        return *this;
    }

    Asn1ContainerFormatter::~Asn1ContainerFormatter()
    {
        if (sizeMarker)
        {
            infra::SavedMarkerDataStream sizeStream(Stream(), sizeMarker);
            AddLength(sizeStream, Stream().ProcessedBytesSince(sizeMarker));
        }
    }
}
