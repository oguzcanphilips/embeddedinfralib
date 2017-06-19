#ifndef ZERO_COPY_DATA_TRANSPORT_HPP
#define ZERO_COPY_DATA_TRANSPORT_HPP

#include "infra/util/ByteRange.hpp"
#include "infra/util/Observer.hpp"
#include "infra/util/BoundedString.hpp"

namespace services
{
    class DataTransport;

    class DataTransportObserver
    {
    protected:
        explicit DataTransportObserver();
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

    class BulkData;

    class BulkDataObserver
        : public services::DataTransportObserver
        , public infra::SingleObserver<BulkDataObserver, BulkData>
    {
    public:
        explicit BulkDataObserver(BulkData& subject);
    };

    class BulkData
        : public services::DataTransport
        , public infra::Subject<BulkDataObserver>
    {
    public:
        virtual void Error(infra::BoundedConstString errorMessage) = 0;
    };
}
#endif
