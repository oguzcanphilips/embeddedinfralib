#ifndef INFRA_ASN1_FORMATTER_HPP
#define INFRA_ASN1_FORMATTER_HPP

#include "infra/stream/public/OutputStream.hpp"
#include "infra/stream/public/SavedMarkerStream.hpp"
#include "infra/util/public/WithStorage.hpp"

namespace infra
{
    class Asn1SequenceFormatter;

    class Asn1Formatter
    {
    public:
        explicit Asn1Formatter(infra::DataOutputStream& stream);

        void Add(uint8_t value);
        void Add(uint32_t value);

        void AddBigNumber(infra::ConstByteRange number);

        void AddContextSpecific(infra::ConstByteRange data);
        void AddObjectId(infra::ConstByteRange oid);

        Asn1SequenceFormatter StartSequence();

        template<typename T>
        void AddOptional(infra::Optional<T> value);

        bool HasFailed() const;

    private:
        enum Type : uint8_t
        {
            Integer = 0x02,
            Null = 0x05,
            Oid = 0x06,
            Sequence = 0x10,
            Set = 0x11,
            Constructed = 0x20,
            ContextSpecific = 0x80
        };

    private:
        template<typename T>
        void AddTag(uint8_t type, uint32_t length, T data);

        void AddTag(uint8_t type, uint32_t length);
        void AddLength(uint32_t length);

    protected:
        infra::DataOutputStream stream;
    };

    template<typename T>
    void Asn1Formatter::AddOptional(infra::Optional<T> value)
    {
        if (value)
            Add(*value);
        else
            AddTag(Type::Null, 0);
    }

    template<typename T>
    void Asn1Formatter::AddTag(uint8_t type, uint32_t length, T data)
    {
        AddTag(type, length);
        stream << data;
    }

    class Asn1SequenceFormatter
        : public Asn1Formatter
    {
    public:
        Asn1SequenceFormatter(infra::DataOutputStream& stream, const uint8_t* sizeMarker);
        Asn1SequenceFormatter(const Asn1SequenceFormatter& other) = delete;
        Asn1SequenceFormatter(Asn1SequenceFormatter&& other);
        Asn1SequenceFormatter& operator=(const Asn1SequenceFormatter& other) = delete;
        Asn1SequenceFormatter& operator=(Asn1SequenceFormatter&& other);
        ~Asn1SequenceFormatter();

    private:
        const uint8_t* sizeMarker;
    };
}

#endif
