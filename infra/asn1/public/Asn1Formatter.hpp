#ifndef INFRA_ASN1_FORMATTER_HPP
#define INFRA_ASN1_FORMATTER_HPP

#include "infra/stream/public/OutputStream.hpp"
#include "infra/util/public/WithStorage.hpp"

namespace infra
{
    class Asn1Formatter
    {
    public:
        explicit Asn1Formatter(infra::DataOutputStream& stream);

        void Add(uint8_t value);
        void Add(uint32_t value);

        void AddBigNumber(infra::ConstByteRange number);

        void AddContextSpecific(infra::ConstByteRange data);
        void AddObjectId(infra::ConstByteRange oid);

        Asn1Formatter StartSequence(uint32_t size);

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
}

#endif
