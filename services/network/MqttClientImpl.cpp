#include "services/network/MqttClientImpl.hpp"

namespace services
{
    MqttClientImpl::MqttFormatter::MqttFormatter(infra::DataOutputStream stream)
        : stream(stream)
    {}

    void MqttClientImpl::MqttFormatter::MessageConnect(infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
    {
        PacketConnect packetHeader {};
        Header(PacketType::packetTypeConnect, sizeof(packetHeader) + EncodedLength(clientId) + EncodedLength(username) + EncodedLength(password));
        stream << packetHeader;
        AddString(clientId);
        AddString(username);
        AddString(password);
    }

    std::size_t MqttClientImpl::MqttFormatter::MessageSizeConnect(infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
    {
        return 5 + sizeof(PacketConnect) + EncodedLength(clientId) + EncodedLength(username) + EncodedLength(password);
    }

    void MqttClientImpl::MqttFormatter::MessagePublish(infra::BoundedConstString topic, infra::BoundedConstString payload)
    {
        Header(PacketType::packetTypePublish, EncodedLength(topic) + EncodedLength(payload));
        AddString(topic);
        stream << infra::StringAsByteRange(payload);
    }

    std::size_t MqttClientImpl::MqttFormatter::MessageSizePublish(infra::BoundedConstString topic, infra::BoundedConstString payload)
    {
        return 5 + EncodedLength(topic) + payload.size();
    }

    std::size_t MqttClientImpl::MqttFormatter::EncodedLength(infra::BoundedConstString value)
    {
        return 2 + value.size();
    }

    void MqttClientImpl::MqttFormatter::AddString(infra::BoundedConstString value)
    {
        stream << BigEndianUint16(static_cast<uint16_t>(value.size())) << infra::StringAsByteRange(value);
    }

    void MqttClientImpl::MqttFormatter::Header(PacketType packetType, std::size_t size, uint8_t flags)
    {
        stream << MakePacketType(packetType, flags);

        while (size > 127)
        {
            stream << static_cast<uint8_t>((size & 0x7f) | 0x80);
            size >>= 8;
        }
        stream << static_cast<uint8_t>(size);
    }

    uint8_t MqttClientImpl::MqttFormatter::MakePacketType(PacketType packetType, uint8_t flags)
    {
        return static_cast<uint8_t>((static_cast<uint8_t>(packetType) << 4) | flags);
    }

    MqttClientImpl::MqttParser::MqttParser(infra::DataInputStream stream)
        : stream(stream)
    {
        ExtractType();
        ExtractSize();
    }

    MqttClientImpl::PacketType MqttClientImpl::MqttParser::GetPacketType() const
    {
        return packetType;
    }

    void MqttClientImpl::MqttParser::ExtractType()
    {
        uint8_t combinedPacketType;
        this->stream >> combinedPacketType;
        packetType = static_cast<PacketType>(combinedPacketType >> 4);
    }

    void MqttClientImpl::MqttParser::ExtractSize()
    {
        uint8_t sizeByte;
        uint8_t shift = 0;
        this->stream >> sizeByte;
        while (!stream.Failed() && sizeByte > 127)
        {
            size += sizeByte << shift;
            shift += 8;
            this->stream >> sizeByte;
        }
        size += sizeByte << shift;
    }

    MqttClientImpl::MqttClientImpl(MqttClientObserverFactory& factory, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
        : state(infra::InPlaceType<StateConnecting>(), *this, factory, clientId, username, password)
    {}

    void MqttClientImpl::Publish(infra::BoundedConstString topic, infra::BoundedConstString payload)
    {
        state->Publish(topic, payload);
    }

    void MqttClientImpl::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        state->SendStreamAvailable(std::move(stream));
    }

    void MqttClientImpl::DataReceived()
    {
        state->DataReceived(*ConnectionObserver::Subject().ReceiveStream());
    }

    void MqttClientImpl::Connected()
    {
        state->Connected();
    }

    MqttClientImpl::StateBase::StateBase(MqttClientImpl& clientConnection)
        : clientConnection(clientConnection)
    {}

    void MqttClientImpl::StateBase::Connected()
    {
        std::abort();
    }

    void MqttClientImpl::StateBase::Publish(infra::BoundedConstString topic, infra::BoundedConstString payload)
    {
        std::abort();
    }

    MqttClientImpl::StateConnecting::StateConnecting(MqttClientImpl& clientConnection, MqttClientObserverFactory& factory, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
        : StateBase(clientConnection)
        , factory(factory)
        , clientId(clientId)
        , username(username)
        , password(password)
        , timeout(std::chrono::minutes(1), [this]() { Timeout(); })
    {}

    void MqttClientImpl::StateConnecting::Connected()
    {
        clientConnection.ConnectionObserver::Subject().RequestSendStream(MqttFormatter::MessageSizeConnect(clientId, username, password));
    }

    void MqttClientImpl::StateConnecting::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        MqttFormatter formatter(*stream);
        formatter.MessageConnect(clientId, username, password);
    }

    void MqttClientImpl::StateConnecting::DataReceived(infra::DataInputStream stream)
    {
        MqttParser parser(stream);
        if (!stream.Failed())
        {
            if (parser.GetPacketType() == PacketType::packetTypeConAck)
            {
                factory.ConnectionEstablished([this, &stream](infra::SharedPtr<MqttClientObserver> client)
                {
                    client->Attach(clientConnection);
                    clientConnection.client = client;
                    auto& newState = clientConnection.state.Emplace<StateConnected>(clientConnection);
                    newState.DataReceived(stream);
                });

                clientConnection.ConnectionObserver::Subject().AckReceived();
            }
            else
            {
                factory.ConnectionFailed(MqttClientObserverFactory::ConnectFailReason::initializationFailed);
                clientConnection.ConnectionObserver::Subject().AbortAndDestroy();
            }
        }
    }

    void MqttClientImpl::StateConnecting::Timeout()
    {
        factory.ConnectionFailed(MqttClientObserverFactory::ConnectFailReason::initializationTimedOut);
        clientConnection.ConnectionObserver::Subject().AbortAndDestroy();
    }

    void MqttClientImpl::StateConnected::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        MqttFormatter formatter(*stream);
        formatter.MessagePublish(topic, payload);

        stream = nullptr;
        clientConnection.GetObserver().PublishDone();
    }

    void MqttClientImpl::StateConnected::DataReceived(infra::DataInputStream stream)
    {}

    void MqttClientImpl::StateConnected::Publish(infra::BoundedConstString topic, infra::BoundedConstString payload)
    {
        this->topic = topic;
        this->payload = payload;
        clientConnection.ConnectionObserver::Subject().RequestSendStream(MqttFormatter::MessageSizePublish(topic, payload));
    }

    MqttClientConnector::MqttClientConnector(MqttClientObserverFactory& factory, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
        : factory(factory)
        , clientId(clientId)
        , username(username)
        , password(password)
    {}

    void MqttClientConnector::ConnectionEstablished(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver)
    {
        createdObserver(connection.Emplace(factory, clientId, username, password));
    }

    void MqttClientConnector::ConnectionFailed(ConnectFailReason reason)
    {
        switch (reason)
        {
            case ConnectFailReason::refused:
                factory.ConnectionFailed(MqttClientObserverFactory::ConnectFailReason::refused);
                break;
            case ConnectFailReason::connectionAllocationFailed:
                factory.ConnectionFailed(MqttClientObserverFactory::ConnectFailReason::connectionAllocationFailed);
                break;
            default:
                std::abort();
        }
    }
}
