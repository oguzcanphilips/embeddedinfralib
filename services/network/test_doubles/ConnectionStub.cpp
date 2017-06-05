#include "services/network/test_doubles/ConnectionStub.hpp"

namespace services
{
    void ZeroCopyConnectionStub::RequestSendStream(std::size_t sendSize)
    {}

    std::size_t ZeroCopyConnectionStub::MaxSendStreamSize() const
    {
        return 1024;
    }

    infra::SharedPtr<infra::DataInputStream> ZeroCopyConnectionStub::ReceiveStream()
    {
        return nullptr;
    }

    void ZeroCopyConnectionStub::AckReceived()
    {}

    void ZeroCopyConnectionStub::CloseAndDestroy()
    {
        ResetOwnership();
    }

    void ZeroCopyConnectionStub::AbortAndDestroy()
    {
        ResetOwnership();
    }

    void ZeroCopyConnectionObserverStub::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>& stream)
    {
        infra::ConstByteRange data(sendData.data(), sendData.data() + requestedSendStreamSize);
        *stream << data;
        stream = nullptr;
        sendData.erase(sendData.begin(), sendData.begin() + requestedSendStreamSize);
        requestedSendStreamSize = 0;
        TryRequestSendStream();
    }

    void ZeroCopyConnectionObserverStub::DataReceived()
    {
        infra::SharedPtr<infra::DataInputStream> stream = Subject().ReceiveStream();

        for (infra::ConstByteRange received = stream->ContiguousRange(); !received.empty(); received = stream->ContiguousRange())
            receivedData.insert(receivedData.end(), received.begin(), received.end());

        Subject().AckReceived();
    }

    void ZeroCopyConnectionObserverStub::SendData(const std::vector<uint8_t>& data)
    {
        sendData.insert(sendData.end(), data.begin(), data.end());

        TryRequestSendStream();
    }

    void ZeroCopyConnectionObserverStub::TryRequestSendStream()
    {
        if (requestedSendStreamSize == 0 && !sendData.empty())
        {
            requestedSendStreamSize = std::min(sendData.size(), Subject().MaxSendStreamSize());
            Subject().RequestSendStream(requestedSendStreamSize);
        }
    }
}
