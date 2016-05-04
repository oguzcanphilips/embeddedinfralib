#ifndef COMCALLBACKMOCK_H
#define COMCALLBACKMOCK_H

#include <vector>
#include "PacketCommunication.hpp"

class CommCallbackMock : public erpc::PacketCommunication::Callback
{
public:
	CommCallbackMock(erpc::PacketCommunication& comm, uint8_t interfaceId, uint16_t len) 
        : erpc::PacketCommunication::Callback(interfaceId)
		, comm(comm)
		, data(len)
	{
		comm.Register(*this);
	}

	~CommCallbackMock()
	{
		comm.Unregister(*this);
	}

    void Receive(erpc::PacketCommunication& pc)
	{
		for (uint16_t i = 0; i < data.size(); ++i)
		{
			pc.Read(data[i]);
		}
	}

	erpc::PacketCommunication& comm;
	std::vector<uint8_t> data;
};

#endif