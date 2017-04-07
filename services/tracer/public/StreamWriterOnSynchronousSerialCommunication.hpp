#ifndef SERVICES_TRACER_STREAM_WRITER_ON_SYNCHRONOUS_SERIAL_COMMUNICATION_HPP
#define SERVICES_TRACER_STREAM_WRITER_ON_SYNCHRONOUS_SERIAL_COMMUNICATION_HPP

#include "hal/synchronous_interfaces/public/SynchronousSerialCommunication.hpp"
#include "infra/stream/public/OutputStream.hpp"

namespace services
{
    class StreamWriterOnSynchronousSerialCommunication
        : public infra::StreamWriter
    {
    public:
        explicit StreamWriterOnSynchronousSerialCommunication(hal::SynchronousSerialCommunication& communication);

        virtual void Insert(infra::ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;
        virtual void Forward(std::size_t amount) override;

    private:
        hal::SynchronousSerialCommunication& communication;
    };
}

#endif