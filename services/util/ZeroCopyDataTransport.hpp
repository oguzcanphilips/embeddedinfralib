#ifndef ZERO_COPY_DATA_TRANSPORT_HPP
#define ZERO_COPY_DATA_TRANSPORT_HPP
#include "infra/util/ByteRange.hpp"
#include "infra/util/Observer.hpp"
#include "infra/util/SharedPtr.hpp"

namespace services
{

    class ZeroCopyDataTransport;

    class ZeroCopyDataTransportObserver
        : public infra::SingleObserver<ZeroCopyDataTransportObserver, ZeroCopyDataTransport>
    {
    protected:
        explicit ZeroCopyDataTransportObserver();
        explicit ZeroCopyDataTransportObserver(ZeroCopyDataTransport& dataTransport);
        ZeroCopyDataTransportObserver(const ZeroCopyDataTransportObserver& other) = delete;
        ZeroCopyDataTransportObserver& operator=(const ZeroCopyDataTransportObserver& other) = delete;
        ~ZeroCopyDataTransportObserver() = default;

    public:
        virtual void SendBufferAvailable(infra::ByteRange sendBuffer) = 0;
        virtual void DataReceived() = 0;

    private:
        friend class ZeroCopyDataTransport;
    };

    class ZeroCopyDataTransport
        : public infra::Subject<ZeroCopyDataTransportObserver>
    {
    protected:
        ZeroCopyDataTransport() = default;
        ZeroCopyDataTransport(const ZeroCopyDataTransport& other) = delete;
        ZeroCopyDataTransport& operator=(const ZeroCopyDataTransport& other) = delete;
        ~ZeroCopyDataTransport() = default;

    public:
        virtual void RequestSendBuffer(std::size_t sendSize) = 0;
        virtual std::size_t MaxSendBufferSize() const = 0;
        virtual infra::ConstByteRange ReceiveBuffer() = 0;
        virtual void AckReceived() = 0;

        void SetOwnership(const infra::SharedPtr<void>& owner, const infra::SharedPtr<ZeroCopyDataTransportObserver>& observer);
        void ResetOwnership();

    private:
        infra::SharedPtr<void> owner;
        infra::SharedPtr<ZeroCopyDataTransportObserver> observer;
    };

}
#endif