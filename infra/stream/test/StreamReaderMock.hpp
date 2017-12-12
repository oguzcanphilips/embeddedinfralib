#ifndef STREAM_READER_MOCK
#define STREAM_READER_MOCK
#include "gmock/gmock.h"
#include "infra/stream/InputStream.hpp"

class StreamReaderMock
	: public infra::StreamReader
{
public:
	using infra::StreamReader::StreamReader;

	MOCK_METHOD1(Extract, void(infra::ByteRange range));
	MOCK_METHOD0(ExtractOne, uint8_t());
	MOCK_METHOD0(Peek, uint8_t());
	MOCK_METHOD1(ExtractContiguousRange, infra::ConstByteRange(std::size_t max));
	MOCK_CONST_METHOD0(Empty, bool());
	MOCK_CONST_METHOD0(Available, std::size_t());
};

#endif