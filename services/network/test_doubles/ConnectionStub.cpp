#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "services/network/test_doubles/ConnectionStub.hpp"

namespace services
{
    void ZeroCopyConnectionStub::RequestSendStream(std::size_t sendSize)
    {
        assert(sendStream.Allocatable());
        sendStreamPtr = sendStream.Emplace(*this);
        infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ZeroCopyConnectionStub>& object) { object->GetObserver().SendStreamAvailable(std::move(object->sendStreamPtr)); }, SharedFromThis());
    }

    std::size_t ZeroCopyConnectionStub::MaxSendStreamSize() const
    {
        return 1024;
    }

    infra::SharedPtr<infra::DataInputStream> ZeroCopyConnectionStub::ReceiveStream()
    {
        assert(receiveStream.Allocatable());
        receivingIndex = 0;
        return receiveStream.Emplace(*this);
    }

    void ZeroCopyConnectionStub::AckReceived()
    {
        receivingData.erase(receivingData.begin(), receivingData.begin() + receivingIndex);
        receivingIndex = 0;
    }

    void ZeroCopyConnectionStub::CloseAndDestroy()
    {
        ResetOwnership();
    }

    void ZeroCopyConnectionStub::AbortAndDestroy()
    {
        ResetOwnership();
    }

    void ZeroCopyConnectionStub::SimulateDataReceived(infra::ConstByteRange data)
    {
        receivingData.insert(receivingData.end(), data.begin(), data.end());
        infra::EventDispatcherWithWeakPtr::Instance().Schedule([this](const infra::SharedPtr<ZeroCopyConnectionStub>& object) { object->GetObserver().DataReceived(); }, SharedFromThis());
    }

    ZeroCopyConnectionStub::SendStreamStub::SendStreamStub(ZeroCopyConnectionStub& connection)
        : infra::DataOutputStream(static_cast<infra::StreamWriter&>(*this))
        , connection(connection)
    {}

    void ZeroCopyConnectionStub::SendStreamStub::Insert(infra::ConstByteRange range)
    {
        connection.sentData.insert(connection.sentData.end(), range.begin(), range.end());
    }

    void ZeroCopyConnectionStub::SendStreamStub::Insert(uint8_t element)
    {
        connection.sentData.push_back(element);
    }

    ZeroCopyConnectionStub::ReceiveStreamStub::ReceiveStreamStub(ZeroCopyConnectionStub& connection)
        : infra::StreamReader(infra::softFail)
        , infra::DataInputStream(static_cast<infra::StreamReader&>(*this))
        , connection(connection)
    {}

    void ZeroCopyConnectionStub::ReceiveStreamStub::Extract(infra::ByteRange range)
    {
        ReportResult(connection.receivingData.size() - connection.receivingIndex >= range.size());
        range.shrink_from_back_to(connection.receivingData.size() - connection.receivingIndex);
        std::copy(connection.receivingData.begin() + connection.receivingIndex, connection.receivingData.begin() + connection.receivingIndex + range.size(), range.begin());
        connection.receivingIndex += range.size();
    }

    uint8_t ZeroCopyConnectionStub::ReceiveStreamStub::ExtractOne()
    {
        uint8_t result;
        Extract(infra::MakeByteRange(result));
        return result;
    }

    uint8_t ZeroCopyConnectionStub::ReceiveStreamStub::Peek()
    {
        ReportResult(connection.receivingData.size() - connection.receivingIndex >= 1);
        if (connection.receivingData.size() - connection.receivingIndex >= 1)
            return connection.receivingData[connection.receivingIndex];
        else
            return 0;
    }

    infra::ConstByteRange ZeroCopyConnectionStub::ReceiveStreamStub::ExtractContiguousRange(std::size_t max)
    {
        infra::ConstByteRange available(connection.receivingData.data() + connection.receivingIndex, connection.receivingData.data() + connection.receivingData.size());
        infra::ConstByteRange result(infra::Head(available, max));
        connection.receivingIndex += result.size();
        return result;
    }

    bool ZeroCopyConnectionStub::ReceiveStreamStub::IsEmpty() const
    {
        return connection.receivingData.size() == connection.receivingIndex;
    }

    std::size_t ZeroCopyConnectionStub::ReceiveStreamStub::SizeAvailable() const
    {
        return connection.receivingData.size() - connection.receivingIndex;
    }
    
    void ZeroCopyConnectionObserverStub::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
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
