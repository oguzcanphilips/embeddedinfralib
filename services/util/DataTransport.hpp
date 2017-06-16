#ifndef ZERO_COPY_DATA_TRANSPORT_HPP
#define ZERO_COPY_DATA_TRANSPORT_HPP

#include "infra/util/ByteRange.hpp"
#include "infra/util/Observer.hpp"
#include "infra/util/SharedPtr.hpp"

namespace services
{
    class DataTransport;

    class DataTransportObserver
        : public infra::SingleObserver<DataTransportObserver, DataTransport>
    {
    protected:
        explicit DataTransportObserver();
        explicit DataTransportObserver(DataTransport& dataTransport);
        DataTransportObserver(const DataTransportObserver& other) = delete;
        DataTransportObserver& operator=(const DataTransportObserver& other) = delete;
        ~DataTransportObserver() = default;

    public:
        virtual void SendBufferAvailable(infra::ByteRange sendBuffer) = 0;
        virtual void DataReceived() = 0;

    private:
        friend class DataTransport;
    };

    class DataTransport
        : public infra::Subject<DataTransportObserver>
    {
    protected:
        DataTransport() = default;
        DataTransport(const DataTransport& other) = delete;
        DataTransport& operator=(const DataTransport& other) = delete;
        ~DataTransport() = default;

    public:
        virtual void RequestSendBuffer(std::size_t sendSize) = 0;
        virtual std::size_t MaxSendBufferSize() const = 0;
        virtual void AckSendBufferFill() = 0;

        virtual infra::ConstByteRange ReceiveBuffer() = 0;
        virtual void AckReceived() = 0;
    };

    class BulkDataObserver;

    class BulkData
        : public services::DataTransport
    {
    };

    class BulkDataObserver
        : services::DataTransportObserver
    {
    public:
        explicit BulkDataObserver(BulkData& subject);
    };
}
#endif
