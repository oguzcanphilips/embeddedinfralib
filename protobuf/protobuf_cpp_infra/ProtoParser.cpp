#include "protobuf/protobuf_cpp_infra/ProtoParser.hpp"
#include <cassert>
#include <cstdlib>

namespace services
{
    ProtoLengthDelimited::ProtoLengthDelimited(infra::DataInputStream inputStream, uint32_t length)
        : limitedReader(inputStream.Reader(), length)
        , input(limitedReader)
    {}

    ProtoLengthDelimited::ProtoLengthDelimited(const ProtoLengthDelimited& other)
        : limitedReader(other.limitedReader)
        , input(limitedReader)
    {}

    void ProtoLengthDelimited::SkipEverything()
    {
        while (!input.Empty())
            input.ContiguousRange();
    }

    ProtoParser ProtoLengthDelimited::Parser()
    {
        return ProtoParser(input);
    }

    void ProtoLengthDelimited::GetString(infra::BoundedString& string)
    {
        string.resize(std::min(input.Available(), string.max_size()));
        assert(string.size() == input.Available());
        input >> infra::StringAsByteRange(string);
    }

    ProtoParser::ProtoParser(infra::DataInputStream inputStream)
        : limitedReader(inputStream.Reader(), inputStream.Available())
        , input(limitedReader)
    {}

    bool ProtoParser::Empty() const
    {
        return input.Empty();
    }

    uint64_t ProtoParser::GetVarInt()
    {
        uint64_t result = 0;
        uint8_t byte;
        uint8_t shift = 0;

        do
        {
            input >> byte;

            result += (byte & 0x7f) << shift;
            shift += 7;
        } while ((byte & 0x80) != 0);

        return result;
    }

    uint32_t ProtoParser::GetFixed32()
    {
        uint32_t result = 0;
        input >> result;
        return result;
    }

    uint64_t ProtoParser::GetFixed64()
    {
        uint64_t result = 0;
        input >> result;
        return result;
    }

    ProtoParser::Field ProtoParser::GetField()
    {
        uint32_t x = static_cast<uint32_t>(GetVarInt());
        uint8_t type = x & 7;
        uint32_t fieldNumber = x >> 3;

        switch (type)
        {
            case 0:
                return std::make_pair(GetVarInt(), fieldNumber);
            case 1:
                return std::make_pair(GetFixed64(), fieldNumber);
            case 2:
                return std::make_pair(ProtoLengthDelimited(input, static_cast<uint32_t>(GetVarInt())), fieldNumber);
            case 5:
                return std::make_pair(GetFixed32(), fieldNumber);
            default:
                std::abort();
        }
    }
}
