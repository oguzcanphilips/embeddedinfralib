#ifndef INFRA_STREAM_READER_MOCK
#define INFRA_STREAM_READER_MOCK

#include "gmock/gmock.h"
#include "infra/stream/InputStream.hpp"

namespace infra
{
    class StreamReaderMock
        : public infra::StreamReader
    {
    public:
        using infra::StreamReader::StreamReader;

        MOCK_METHOD2(Extract, void(ByteRange range, StreamErrorPolicy& errorPolicy));
        MOCK_METHOD1(Peek, uint8_t(StreamErrorPolicy& errorPolicy));
        MOCK_METHOD1(ExtractContiguousRange, infra::ConstByteRange(std::size_t max));
        MOCK_CONST_METHOD0(Empty, bool());
        MOCK_CONST_METHOD0(Available, std::size_t());
    };
}

#endif