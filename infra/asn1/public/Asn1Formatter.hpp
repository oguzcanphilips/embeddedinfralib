#ifndef INFRA_ASN1_FORMATTER_HPP
#define INFRA_ASN1_FORMATTER_HPP

#include "infra/stream/public/OutputStream.hpp"
#include "infra/stream/public/SavedMarkerStream.hpp"
#include "infra/util/public/WithStorage.hpp"

namespace infra
{
    class Asn1ContainerFormatter;

    class Asn1Formatter
    {
    public:
        explicit Asn1Formatter(infra::DataOutputStream& stream);

        void Add(uint8_t value);
        void Add(uint32_t value);
        void Add(int32_t value);

        void AddSerial(infra::ConstByteRange serial);
        void AddBigNumber(infra::ConstByteRange number);
        void AddContextSpecific(uint8_t context, infra::ConstByteRange data);
        void AddObjectId(infra::ConstByteRange oid);
        void AddBitString(infra::ConstByteRange string);
        void AddPrintableString(infra::ConstByteRange string);
        void AddUtcTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

        Asn1ContainerFormatter StartSequence();
        Asn1ContainerFormatter StartSet();
        Asn1ContainerFormatter StartContextSpecific(uint8_t context = 0);
        Asn1ContainerFormatter StartBitString();

        template<typename T>
        void AddOptional(infra::Optional<T> value);

        bool HasFailed() const;

    protected:
        infra::DataOutputStream& Stream();

    private:
        enum Type : uint8_t
        {
            Integer = 0x02,
            BitString = 0x03,
            Null = 0x05,
            Oid = 0x06,
            Sequence = 0x10,
            Set = 0x11,
            UtcTime = 0x17,
            PrintableString = 0x13,
            UniversalString = 0x1C,
            Constructed = 0x20,
            ContextSpecific = 0x80
        };

    private:
        template<typename T>
        void AddTag(uint8_t type, uint32_t length, T data);
        void AddTag(uint8_t type, uint32_t length);

    private:
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

    class Asn1ContainerFormatter
        : public Asn1Formatter
    {
    public:
        Asn1ContainerFormatter(infra::DataOutputStream& stream, const uint8_t* sizeMarker);
        Asn1ContainerFormatter(const Asn1ContainerFormatter& other) = delete;
        Asn1ContainerFormatter(Asn1ContainerFormatter&& other);
        Asn1ContainerFormatter& operator=(const Asn1ContainerFormatter& other) = delete;
        Asn1ContainerFormatter& operator=(Asn1ContainerFormatter&& other);
        ~Asn1ContainerFormatter();

    private:
        const uint8_t* sizeMarker;
    };
}

#endif
