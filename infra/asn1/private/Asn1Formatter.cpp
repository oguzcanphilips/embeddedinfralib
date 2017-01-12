#include "infra/asn1/public/Asn1Formatter.hpp"

namespace infra
{
    Asn1Formatter::Asn1Formatter(infra::ByteOutputStream& stream)
        : stream(&stream)
    {}

    Asn1Formatter::Asn1Formatter(Asn1Formatter&& other)
        : stream(other.stream)
    {}

    Asn1Formatter& Asn1Formatter::operator=(Asn1Formatter&& other)
    {
        stream = other.stream;

        return *this;
    }

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

    Asn1Formatter Asn1Formatter::StartSequence(uint32_t size)
    {
        AddTag(Type::Constructed | Type::Sequence, size);

        return Asn1Formatter(*stream);
    }

    bool Asn1Formatter::HasFailed() const
    {
        return stream->HasFailed();
    }

    void Asn1Formatter::AddTag(uint8_t type, uint32_t length)
    {
        *stream << type;
        AddLength(length);
    }

    void Asn1Formatter::AddLength(uint32_t length)
    {
        if (length > 0xFF)
            *stream << uint8_t(0x82) << uint8_t((length & 0xFF00) >> 8) << uint8_t(length & 0xFF);
        else if (length > 0x7F)
            *stream << uint8_t(0x81) << uint8_t(length & 0xFF);
        else
            *stream << uint8_t(length & 0xFF);
    }
}
