#ifndef SERVICES_DATAGRAM_HPP
#define SERVICES_DATAGRAM_HPP

#include "infra/stream/InputStream.hpp"
#include "infra/stream/OutputStream.hpp"
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
		virtual void DataReceived(infra::DataInputStream stream, IPv4Address address) = 0;
	};

    class DatagramProvider
    {
    protected:
        DatagramProvider() = default;
        DatagramProvider(const DatagramProvider& other) = delete;
        DatagramProvider& operator=(const DatagramProvider& other) = delete;
        virtual ~DatagramProvider() = default;

    public:
        virtual infra::SharedPtr<DatagramSender> Connect(DatagramSenderObserver& sender, IPv4Address address, uint16_t port) = 0;
        virtual infra::SharedPtr<void> Listen(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed) = 0;
    };
}

#endif
