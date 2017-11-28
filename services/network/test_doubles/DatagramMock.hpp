#ifndef DATAGRAM_MOCK_HPP
#define	DATAGRAM_MOCK_HPP

#include "gmock/gmock.h"
#include "services/network/Datagram.hpp"

namespace services
{
	class DatagramProviderMock
		: public DatagramProvider
	{
	public:
		MOCK_METHOD3(Connect, infra::SharedPtr<DatagramSender>(DatagramSenderObserver& sender, IPv4Address address, uint16_t port));
		MOCK_METHOD3(Listen, infra::SharedPtr<void>(DatagramReceiver& receiver, uint16_t port, bool broadcastAllowed));
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
		MOCK_METHOD2(DataReceived, void(infra::DataInputStream stream, IPv4Address address));
	};
}
#endif
