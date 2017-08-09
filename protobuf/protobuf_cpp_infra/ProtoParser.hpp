#ifndef PROTOBUF_PROTO_PARSER_HPP
#define PROTOBUF_PROTO_PARSER_HPP

#include "infra/stream/InputStream.hpp"
#include "infra/stream/LimitedInputStream.hpp"
#include "infra/util/Variant.hpp"
#include <utility>

namespace services
{
    class ProtoParser;

    class ProtoLengthDelimited
    {
    public:
        ProtoLengthDelimited(infra::DataInputStream inputStream, uint32_t length);

        ProtoLengthDelimited(const ProtoLengthDelimited& other);

        void SkipEverything();
        ProtoParser Parser();
        void GetString(infra::BoundedString& string);

    private:
        infra::LimitedStreamReader limitedReader;
        infra::DataInputStream input;
    };

    class ProtoParser
    {
    public:
        using Field = std::pair<infra::Variant<uint32_t, uint64_t, ProtoLengthDelimited>, uint32_t>;

        explicit ProtoParser(infra::DataInputStream inputStream);

        bool Empty() const;
        uint64_t GetVarInt();
        uint32_t GetFixed32();
        uint64_t GetFixed64();

        Field GetField();

    private:
        infra::LimitedStreamReader limitedReader;
        infra::DataInputStream input;
    };
}

#endif