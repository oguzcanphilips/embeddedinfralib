#ifndef SERVICES_STREAM_WRITER_ON_SERIAL_COMMUNICATION_HPP
#define SERVICES_STREAM_WRITER_ON_SERIAL_COMMUNICATION_HPP

#include "hal/interfaces/public/SerialCommunication.hpp"
#include "infra/stream/public/OutputStream.hpp"
#include "infra/util/public/CyclicBuffer.hpp"

namespace services
{
    class StreamWriterOnSerialCommunication
        : public infra::StreamWriter
    {
    public:
        template<std::size_t StorageSize>
            using WithStorage = infra::WithStorage<StreamWriterOnSerialCommunication, std::array<uint8_t, StorageSize>>;

        StreamWriterOnSerialCommunication(infra::ByteRange bufferStorage, hal::SerialCommunication& communication);

        virtual void Insert(infra::ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;
        virtual void Forward(std::size_t amount) override;

    private:
        void TrySend();
        void CommunicationDone(uint32_t size);

    private:
        infra::CyclicByteBuffer buffer;
        hal::SerialCommunication& communication;
        bool communicating = false;
    };
}

#endif