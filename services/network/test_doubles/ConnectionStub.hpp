#ifndef NETWORK_CONNECTION_STUB_HPP
#define NETWORK_CONNECTION_STUB_HPP

#include "gmock/gmock.h"
#include "infra/util/SharedOptional.hpp"
#include "services/network/Connection.hpp"
#include <vector>

namespace services
{
    //TICS -INT#002: A mock or stub may have public data
    //TICS -INT#027: A MOCK macro should not add virtual to its signature
    class ConnectionStub
        : public services::Connection
        , public infra::EnableSharedFromThis<ConnectionStub>
    {
    public:
        virtual void RequestSendStream(std::size_t sendSize) override;
        virtual std::size_t MaxSendStreamSize() const override;
        virtual infra::SharedPtr<infra::DataInputStream> ReceiveStream() override;
        virtual void AckReceived() override;
        virtual void CloseAndDestroy() override;
        virtual void AbortAndDestroy() override;

        MOCK_METHOD0(CloseAndDestroyMock, void());
        MOCK_METHOD0(AbortAndDestroyMock, void());
        MOCK_CONST_METHOD0(Ipv4Address, IPv4Address());

        void SimulateDataReceived(infra::ConstByteRange data);

        std::vector<uint8_t> sentData;

    private:
        class SendStreamStub
            : public infra::DataOutputStream
            , private infra::StreamWriter
        {
        public:
            explicit SendStreamStub(ConnectionStub& connection);

        private:
            virtual void Insert(infra::ConstByteRange range) override;
            virtual void Insert(uint8_t element) override;

        private:
            ConnectionStub& connection;
        };

        class ReceiveStreamStub
            : public infra::DataInputStream
            , private infra::StreamReader
        {
        public:
            explicit ReceiveStreamStub(ConnectionStub& connection);

        private:
            virtual void Extract(infra::ByteRange range) override;
            virtual uint8_t ExtractOne() override;
            virtual uint8_t Peek() override;
            virtual infra::ConstByteRange ExtractContiguousRange(std::size_t max) override;
            virtual bool IsEmpty() const override;
            virtual std::size_t SizeAvailable() const override;

        private:
            ConnectionStub& connection;
        };

    private:
        std::vector<uint8_t> receivingData;
        std::size_t receivingIndex = 0;

        infra::SharedOptional<ReceiveStreamStub> receiveStream;
        infra::SharedOptional<SendStreamStub> sendStream;
        infra::SharedPtr<infra::DataOutputStream> sendStreamPtr;
    };

    class ConnectionObserverStub
        : public services::ConnectionObserver
    {
    public:
        ConnectionObserverStub(services::Connection& connection)
            : services::ConnectionObserver(connection)
        {}

        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) override;
        virtual void DataReceived() override;

        void SendData(const std::vector<uint8_t>& data);

        std::vector<uint8_t> receivedData;

        using services::ConnectionObserver::Subject;

    private:
        void TryRequestSendStream();

    private:
        std::size_t requestedSendStreamSize = 0;
        std::vector<uint8_t> sendData;
    };
}

#endif
