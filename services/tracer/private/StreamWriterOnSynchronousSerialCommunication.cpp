#include "services/tracer/public/StreamWriterOnSynchronousSerialCommunication.hpp"

namespace services
{
    StreamWriterOnSynchronousSerialCommunication::StreamWriterOnSynchronousSerialCommunication(hal::SynchronousSerialCommunication& communication)
        : communication(communication)
    {}

    void StreamWriterOnSynchronousSerialCommunication::Insert(infra::ConstByteRange range)
    {
        communication.SendData(range);
    }

    void StreamWriterOnSynchronousSerialCommunication::Insert(uint8_t element)
    {
        Insert(infra::MakeByteRange(element));
    }

    void StreamWriterOnSynchronousSerialCommunication::Forward(std::size_t amount)
    {}
}
