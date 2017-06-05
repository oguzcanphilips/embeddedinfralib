#ifndef NETWORK_CONNECTION_STUB_HPP
#define NETWORK_CONNECTION_STUB_HPP

#include "services/network/Connection.hpp"
#include <vector>

namespace services
{
    class ZeroCopyConnectionStub
        : public services::ZeroCopyConnection
    {
    public:
        virtual void RequestSendStream(std::size_t sendSize) override;
        virtual std::size_t MaxSendStreamSize() const override;
        virtual infra::SharedPtr<infra::DataInputStream> ReceiveStream() override;
        virtual void AckReceived() override;
        virtual void CloseAndDestroy() override;
        virtual void AbortAndDestroy() override;
    };

    class ZeroCopyConnectionObserverStub
        : public services::ZeroCopyConnectionObserver
    {
    public:
        ZeroCopyConnectionObserverStub(services::ZeroCopyConnection& connection)
            : services::ZeroCopyConnectionObserver(connection)
        {}

        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>& stream) override;
        virtual void DataReceived() override;

        void SendData(const std::vector<uint8_t>& data);

        std::vector<uint8_t> receivedData;

        using services::ZeroCopyConnectionObserver::Subject;

    private:
        void TryRequestSendStream();

    private:
        std::size_t requestedSendStreamSize = 0;
        std::vector<uint8_t> sendData;
    };
}

#endif
