#include "services/tracer/StreamWriterOnSynchronousSerialCommunication.hpp"

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

    size_t StreamWriterOnSynchronousSerialCommunication::Available() const
    {
        return std::numeric_limits<size_t>::max();
    }

}
