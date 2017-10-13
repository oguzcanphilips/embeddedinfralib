#include "services/tracer/TracingInputStream.hpp"

namespace services
{
    TracingStreamReader::TracingStreamReader(infra::StreamReader& reader, services::Tracer& tracer)
        : reader(reader)
        , tracer(tracer)
    {}

    void TracingStreamReader::Extract(infra::ByteRange range)
    {
        reader.Extract(range);
        tracer.Trace() << "" << infra::AsHex(range);
    }

    uint8_t TracingStreamReader::ExtractOne()
    {
        uint8_t result = reader.ExtractOne();
        tracer.Trace() << "" << infra::AsHex(infra::MakeByteRange(result));
        return result;
    }

    uint8_t TracingStreamReader::Peek()
    {
        return reader.Peek();
    }

    infra::ConstByteRange TracingStreamReader::ExtractContiguousRange(std::size_t max)
    {
        infra::ConstByteRange result = reader.ExtractContiguousRange(max);
        tracer.Trace() << "" << infra::AsHex(result);
        return result;
    }

    bool TracingStreamReader::Empty() const
    {
        return reader.Empty();
    }

    std::size_t TracingStreamReader::Available() const
    {
        return reader.Available();
    }

    bool TracingStreamReader::Failed() const
    {
        return reader.Failed();
    }

    void TracingStreamReader::ReportResult(bool ok)
    {
        reader.ReportResult(ok);
    }

    void TracingStreamReader::SetSoftFail()
    {
        reader.SetSoftFail();
    }

    TracingInputStream::TracingInputStream(infra::DataInputStream& stream, services::Tracer& tracer)
        : infra::DataInputStream::WithReader<TracingStreamReader>(stream.Reader(), tracer)
    {}
}
