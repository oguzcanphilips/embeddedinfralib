#ifndef INFRA_STREAM_READER_MOCK
#define INFRA_STREAM_READER_MOCK

#include "gmock/gmock.h"
#include "infra/stream/InputStream.hpp"
#include "infra/stream/OutputStream.hpp"

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

    class StreamWriterMock
        : public infra::StreamWriter
    {
    public:
        using infra::StreamWriter::StreamWriter;

        MOCK_METHOD2(Insert, void(ConstByteRange range, StreamErrorPolicy& errorPolicy));
        MOCK_CONST_METHOD0(Available, std::size_t());
        MOCK_CONST_METHOD0(ConstructSaveMarker, const uint8_t*());
        MOCK_CONST_METHOD1(GetProcessedBytesSince, std::size_t(const uint8_t* marker));
        MOCK_METHOD1(SaveState, infra::ByteRange(const uint8_t* marker));
        MOCK_METHOD1(RestoreState, void(infra::ByteRange range));
        MOCK_METHOD1(Overwrite, infra::ByteRange(const uint8_t* marker));
    };
}

#endif
