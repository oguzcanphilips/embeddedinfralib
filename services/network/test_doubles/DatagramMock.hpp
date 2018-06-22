#ifndef NETWORK_DATAGRAM_MOCK_HPP
#define NETWORK_DATAGRAM_MOCK_HPP

#include "gmock/gmock.h"
#include "services/network/Datagram.hpp"

namespace services
{
    class DatagramProviderMock
        : public DatagramProvider
    {
    public:
        MOCK_METHOD3(ListenIPv4, infra::SharedPtr<void>(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed));
        MOCK_METHOD3(ConnectIPv4, infra::SharedPtr<DatagramSender>(DatagramSenderObserver& sender, IPv4Address address, uint16_t port));
        MOCK_METHOD2(ListenIPv6, infra::SharedPtr<void>(DatagramReceiver& receiver, uint16_t port));
        MOCK_METHOD3(ConnectIPv6, infra::SharedPtr<DatagramSender>(DatagramSenderObserver& sender, IPv6Address address, uint16_t port));
    };

    class DatagramSenderMock
        : public DatagramSender
    {
    public:
        MOCK_METHOD1(RequestSendStream, void(std::size_t));
    };

    class DatagramReceiverMock
        : public DatagramReceiver
    {
    public:
        MOCK_METHOD3(DataReceived, void(infra::DataInputStream stream, IPv4Address address, uint16_t port));
        MOCK_METHOD3(DataReceived, void(infra::DataInputStream stream, IPv6Address address, uint16_t port));
    };

    class DatagramExchangeMock
        : public DatagramExchange
    {
    public:
        MOCK_METHOD1(RequestSendStream, void(std::size_t sendSize));
        MOCK_METHOD2(RequestSendStream, void(std::size_t sendSize, UdpSocket to));
    };

    class DatagramFactoryMock
        : public DatagramFactory
    {
    public:
        MOCK_METHOD3(ListenIPv4, infra::SharedPtr<DatagramExchange>(DatagramExchangeObserver& observer, uint16_t port, bool broadcastAllowed));
        MOCK_METHOD2(ConnectIPv4, infra::SharedPtr<DatagramExchange>(DatagramExchangeObserver& observer, Udpv4Socket remote));
        MOCK_METHOD4(ConnectIPv4, infra::SharedPtr<DatagramExchange>(DatagramExchangeObserver& observer, uint16_t localPort, Udpv4Socket remote, bool broadcastAllowed));
        MOCK_METHOD2(ListenIPv6, infra::SharedPtr<DatagramExchange>(DatagramExchangeObserver& observer, uint16_t port));
        MOCK_METHOD2(ConnectIPv6, infra::SharedPtr<DatagramExchange>(DatagramExchangeObserver& observer, Udpv6Socket remote));
        MOCK_METHOD3(ConnectIPv6, infra::SharedPtr<DatagramExchange>(DatagramExchangeObserver& observer, uint16_t localPort, Udpv6Socket remote));
    };
}

#endif
