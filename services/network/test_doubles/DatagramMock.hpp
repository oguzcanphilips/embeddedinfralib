#ifndef DATAGRAM_MOCK_HPP
#define DATAGRAM_MOCK_HPP

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
}

#endif
