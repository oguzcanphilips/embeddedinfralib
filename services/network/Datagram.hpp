#ifndef SERVICES_DATAGRAM_HPP
#define SERVICES_DATAGRAM_HPP

#include "infra/stream/InputStream.hpp"
#include "infra/stream/OutputStream.hpp"
#include "infra/util/Observer.hpp"
#include "infra/util/SharedPtr.hpp"
#include "services/network/Address.hpp"

namespace services
{
    class DatagramSenderObserver
    {
    protected:
        DatagramSenderObserver() = default;
        DatagramSenderObserver(const DatagramSenderObserver& other) = delete;
        DatagramSenderObserver& operator=(const DatagramSenderObserver& other) = delete;
        virtual ~DatagramSenderObserver() = default;

    public:
        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) = 0;
    };

    class DatagramSender
    {
    protected:
        DatagramSender() = default;
        DatagramSender(const DatagramSender& other) = delete;
        DatagramSender& operator=(const DatagramSender& other) = delete;
        virtual ~DatagramSender() = default;

    public:
        virtual void RequestSendStream(std::size_t sendSize) = 0;
    };

    class DatagramReceiver
    {
    protected:
        DatagramReceiver() = default;
        DatagramReceiver(const DatagramReceiver& other) = delete;
        DatagramReceiver& operator=(const DatagramReceiver& other) = delete;
        virtual ~DatagramReceiver() = default;

    public:
        virtual void DataReceived(infra::DataInputStream stream, IPv4Address address, uint16_t port) = 0;
        virtual void DataReceived(infra::DataInputStream stream, IPv6Address address, uint16_t port) = 0;
    };

    class DatagramProvider
    {
    protected:
        DatagramProvider() = default;
        DatagramProvider(const DatagramProvider& other) = delete;
        DatagramProvider& operator=(const DatagramProvider& other) = delete;
        virtual ~DatagramProvider() = default;

    public:
        virtual infra::SharedPtr<void> ListenIPv4(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed) = 0;
        virtual infra::SharedPtr<DatagramSender> ConnectIPv4(DatagramSenderObserver& sender, IPv4Address address, uint16_t port) = 0;
        virtual infra::SharedPtr<void> ListenIPv6(DatagramReceiver& receiver, uint16_t port) = 0;
        virtual infra::SharedPtr<DatagramSender> ConnectIPv6(DatagramSenderObserver& sender, IPv6Address address, uint16_t port) = 0;
    };

    class DatagramExchange;

    class DatagramExchangeObserver
        : public infra::SingleObserver<DatagramExchangeObserver, DatagramExchange>
    {
    public:
        virtual void DataReceived(infra::DataInputStream stream, UdpSocket from) = 0;

        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) = 0;
    };

    class DatagramExchange
        : public infra::Subject<DatagramExchangeObserver>
    {
    public:
        // RequestStream without address is only possible on DatagramExchange objects obtained via a Connect call
        virtual void RequestSendStream(std::size_t sendSize) = 0;

        // RequestStream with address is possible on all DatagramExchange objects
        virtual void RequestSendStream(std::size_t sendSize, UdpSocket to) = 0;
    };

    class DatagramFactory
    {
    protected:
        DatagramFactory() = default;
        DatagramFactory(const DatagramFactory& other) = delete;
        DatagramFactory& operator=(const DatagramFactory& other) = delete;
        virtual ~DatagramFactory() = default;

    public:
        // DatagramExchange objects obtained with a Connect call have a random local port if not explicitly specified
        virtual infra::SharedPtr<DatagramExchange> ListenIPv4(DatagramExchangeObserver& observer, uint16_t port, bool broadcastAllowed) = 0;
        virtual infra::SharedPtr<DatagramExchange> ConnectIPv4(DatagramExchangeObserver& observer, Udpv4Socket remote) = 0;
        virtual infra::SharedPtr<DatagramExchange> ConnectIPv4(DatagramExchangeObserver& observer, uint16_t localPort, Udpv4Socket remote, bool broadcastAllowed) = 0;
        virtual infra::SharedPtr<DatagramExchange> ListenIPv6(DatagramExchangeObserver& observer, uint16_t port) = 0;
        virtual infra::SharedPtr<DatagramExchange> ConnectIPv6(DatagramExchangeObserver& observer, Udpv6Socket remote) = 0;
        virtual infra::SharedPtr<DatagramExchange> ConnectIPv6(DatagramExchangeObserver& observer, uint16_t localPort, Udpv6Socket remote) = 0;
    };
}

#endif
