#include "gmock/gmock.h"
#include "infra/stream/ByteOutputStream.hpp"
#include "protobuf/protobuf_infra/ProtoFormatter.hpp"

TEST(ProtoFormatterTest, PutUint32_as_single_byte)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutUint32(2, 4);
    EXPECT_EQ((std::array<uint8_t, 2>{ 4 << 3, 2 }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, PutUint32_as_multiple_bytes)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutUint32(389, 4);
    EXPECT_EQ((std::array<uint8_t, 3>{ 4 << 3, 0x85, 3 }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, PutString)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutString("a", 4);
    EXPECT_EQ((std::array<uint8_t, 3>{ 4 << 3 | 2, 1, 'a' }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, PutSubObject)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    {
        services::ProtoLengthDelimitedFormatter subObjectFormatter = formatter.LengthDelimitedFormatter(4);
        formatter.PutUint32(2, 4);
    }

    EXPECT_EQ((std::array<uint8_t, 4>{ 4 << 3 | 2, 2, 4 << 3, 2 }), stream.Writer().Processed());
}
