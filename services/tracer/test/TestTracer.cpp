#include "gmock/gmock.h"
#include "infra/stream/public/StringOutputStream.hpp"
#include "services/tracer/public/Tracer.hpp"

class TracerTestImpl
    : public services::Tracer
{
public:
    TracerTestImpl(infra::TextOutputStream& stream)
        : services::Tracer(stream)
    {}

    MOCK_METHOD0(InsertHeader, void());
};

TEST(TracerTest, trace_inserts_header)
{
    infra::StringOutputStream::WithStorage<32> stream;
    testing::StrictMock<TracerTestImpl> tracer(stream);

    EXPECT_CALL(tracer, InsertHeader());
    tracer.Trace();
}

TEST(TracerTest, trace_streams_text)
{
    infra::StringOutputStream::WithStorage<32> stream;
    testing::StrictMock<TracerTestImpl> tracer(stream);
    EXPECT_CALL(tracer, InsertHeader());

    tracer.Trace() << "Text";

    EXPECT_EQ("\r\nText", stream.Storage());
}