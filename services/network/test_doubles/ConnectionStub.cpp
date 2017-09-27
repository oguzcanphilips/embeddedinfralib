#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "services/network/test_doubles/ConnectionStub.hpp"

namespace services
{
    void ConnectionStub::RequestSendStream(std::size_t sendSize)
    {
        assert(sendStream.Allocatable());
        sendStreamPtr = sendStream.Emplace(*this);
        infra::EventDispatcherWithWeakPtr::Instance().Schedule([](const infra::SharedPtr<ConnectionStub>& object) { object->GetObserver().SendStreamAvailable(std::move(object->sendStreamPtr)); }, SharedFromThis());
    }

    std::size_t ConnectionStub::MaxSendStreamSize() const
    {
        return 1024;
    }

    infra::SharedPtr<infra::DataInputStream> ConnectionStub::ReceiveStream()
    {
        assert(receiveStream.Allocatable());
        receivingIndex = 0;
        return receiveStream.Emplace(*this);
    }

    void ConnectionStub::AckReceived()
    {
        receivingData.erase(receivingData.begin(), receivingData.begin() + receivingIndex);
        receivingIndex = 0;
    }

    void ConnectionStub::CloseAndDestroy()
    {
        CloseAndDestroyMock();
        ResetOwnership();
    }

    void ConnectionStub::AbortAndDestroy()
    {
        AbortAndDestroyMock();
        ResetOwnership();
    }

    void ConnectionStub::SimulateDataReceived(infra::ConstByteRange data)
    {
        receivingData.insert(receivingData.end(), data.begin(), data.end());
        infra::EventDispatcherWithWeakPtr::Instance().Schedule([this](const infra::SharedPtr<ConnectionStub>& object)
        {
            if (object->HasObserver())
                object->GetObserver().DataReceived();
        }, SharedFromThis());
    }

    ConnectionStub::StreamWriterStub::StreamWriterStub(ConnectionStub& connection)
        : connection(connection)
    {}

    void ConnectionStub::StreamWriterStub::Insert(infra::ConstByteRange range)
    {
        connection.sentData.insert(connection.sentData.end(), range.begin(), range.end());
    }

    void ConnectionStub::StreamWriterStub::Insert(uint8_t element)
    {
        connection.sentData.push_back(element);
    }

    std::size_t ConnectionStub::StreamWriterStub::Available() const
    {
        return connection.MaxSendStreamSize();
    }

    ConnectionStub::StreamReaderStub::StreamReaderStub(ConnectionStub& connection)
        : infra::StreamReader(infra::softFail)
        , connection(connection)
    {}

    void ConnectionStub::StreamReaderStub::Extract(infra::ByteRange range)
    {
        ReportResult(connection.receivingData.size() - connection.receivingIndex >= range.size());
        range.shrink_from_back_to(connection.receivingData.size() - connection.receivingIndex);
        std::copy(connection.receivingData.begin() + connection.receivingIndex, connection.receivingData.begin() + connection.receivingIndex + range.size(), range.begin());
        connection.receivingIndex += range.size();
    }

    uint8_t ConnectionStub::StreamReaderStub::ExtractOne()
    {
        uint8_t result;
        Extract(infra::MakeByteRange(result));
        return result;
    }

    uint8_t ConnectionStub::StreamReaderStub::Peek()
    {
        ReportResult(connection.receivingData.size() - connection.receivingIndex >= 1);
        if (connection.receivingData.size() - connection.receivingIndex >= 1)
            return connection.receivingData[connection.receivingIndex];
        else
            return 0;
    }

    infra::ConstByteRange ConnectionStub::StreamReaderStub::ExtractContiguousRange(std::size_t max)
    {
        infra::ConstByteRange available(connection.receivingData.data() + connection.receivingIndex, connection.receivingData.data() + connection.receivingData.size());
        infra::ConstByteRange result(infra::Head(available, max));
        connection.receivingIndex += result.size();
        return result;
    }

    bool ConnectionStub::StreamReaderStub::Empty() const
    {
        return connection.receivingData.size() == connection.receivingIndex;
    }

    std::size_t ConnectionStub::StreamReaderStub::Available() const
    {
        return connection.receivingData.size() - connection.receivingIndex;
    }
    
    void ConnectionObserverStub::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        infra::ConstByteRange data(sendData.data(), sendData.data() + requestedSendStreamSize);
        *stream << data;
        stream = nullptr;
        sendData.erase(sendData.begin(), sendData.begin() + requestedSendStreamSize);
        requestedSendStreamSize = 0;
        TryRequestSendStream();
    }

    void ConnectionObserverStub::DataReceived()
    {
        infra::SharedPtr<infra::DataInputStream> stream = Subject().ReceiveStream();

        for (infra::ConstByteRange received = stream->ContiguousRange(); !received.empty(); received = stream->ContiguousRange())
            receivedData.insert(receivedData.end(), received.begin(), received.end());

        Subject().AckReceived();
    }

    void ConnectionObserverStub::SendData(const std::vector<uint8_t>& data)
    {
        sendData.insert(sendData.end(), data.begin(), data.end());

        TryRequestSendStream();
    }

    void ConnectionObserverStub::TryRequestSendStream()
    {
        if (requestedSendStreamSize == 0 && !sendData.empty())
        {
            requestedSendStreamSize = std::min(sendData.size(), Subject().MaxSendStreamSize());
            Subject().RequestSendStream(requestedSendStreamSize);
        }
    }
}
