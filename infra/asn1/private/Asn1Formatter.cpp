#include "infra/asn1/public/Asn1Formatter.hpp"

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
        AddTag(Type::Integer, sizeof(uint32_t), value);
    }

    void Asn1Formatter::AddBigNumber(infra::ConstByteRange number)
    {
        AddTag(Type::Integer, number.size(), number);
    }

    void Asn1Formatter::AddContextSpecific(infra::ConstByteRange data)
    {
        AddTag(Type::Constructed | Type::ContextSpecific, data.size(), data);
    }

    void Asn1Formatter::AddObjectId(infra::ConstByteRange oid)
    {
        AddTag(Type::Oid, oid.size(), oid);
    }

    Asn1SequenceFormatter Asn1Formatter::StartSequence()
    {
        stream << static_cast<uint8_t>(Type::Constructed | Type::Sequence);

        return Asn1SequenceFormatter(stream, stream.SaveMarker());
    }

    bool Asn1Formatter::HasFailed() const
    {
        return stream.HasFailed();
    }

    void Asn1Formatter::AddTag(uint8_t type, uint32_t length)
    {
        stream << type;
        AddLength(length);
    }

    void Asn1Formatter::AddLength(uint32_t length)
    {
        if (length > 0xFF)
            stream << uint8_t(0x82) << uint8_t((length & 0xFF00) >> 8) << uint8_t(length & 0xFF);
        else if (length > 0x7F)
            stream << uint8_t(0x81) << uint8_t(length & 0xFF);
        else
            stream << uint8_t(length & 0xFF);
    }

    Asn1SequenceFormatter::Asn1SequenceFormatter(infra::DataOutputStream& stream, const uint8_t* sizeMarker)
        : Asn1Formatter(stream)
        , sizeMarker(sizeMarker)
    {}

    Asn1SequenceFormatter::Asn1SequenceFormatter(Asn1SequenceFormatter&& other)
        : Asn1Formatter(stream)
        , sizeMarker(other.sizeMarker)
    {
        other.sizeMarker = nullptr;
    }

    Asn1SequenceFormatter& Asn1SequenceFormatter::operator=(Asn1SequenceFormatter&& other)
    {
        sizeMarker = other.sizeMarker;
        other.sizeMarker = nullptr;

        return *this;
    }

    Asn1SequenceFormatter::~Asn1SequenceFormatter()
    {
        if (sizeMarker)
        {
            infra::SavedMarkerDataStream sizeStream(stream, sizeMarker);
            sizeStream << uint8_t(stream.ProcessedBytesSince(sizeMarker));
        }
    }
}
