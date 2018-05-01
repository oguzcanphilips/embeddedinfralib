#include "services/network/Mqtt.hpp"

namespace services
{
    MqttClientConnection::MqttFormatter::MqttFormatter(infra::DataOutputStream stream)
        : stream(stream)
    {}

    template<MqttClientConnection::PacketType packetType, class... Args>
    void MqttClientConnection::MqttFormatter::Message(Args&&... args)
    {
        MessageImpl(InPlaceType<packetType>(), std::forward<Args>(args)...);
    }

    template<MqttClientConnection::PacketType packetType, class... Args>
    std::size_t MqttClientConnection::MqttFormatter::MessageSize(Args&&... args)
    {
        return MessageSizeImpl(InPlaceType<packetType>(), std::forward<Args>(args)...);
    }

    void MqttClientConnection::MqttFormatter::MessageImpl(InPlaceType<PacketType::packetTypeConnect>, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
    {
        PacketConnect packetHeader {};
        Header(PacketType::packetTypeConnect, sizeof(packetHeader) + EncodedLength(clientId) + EncodedLength(username) + EncodedLength(password));
        stream << packetHeader;
        AddString(clientId);
        AddString(username);
        AddString(password);
    }

    std::size_t MqttClientConnection::MqttFormatter::MessageSizeImpl(InPlaceType<PacketType::packetTypeConnect>, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
    {
        return 5 + sizeof(PacketConnect) + EncodedLength(clientId) + EncodedLength(username) + EncodedLength(password);
    }

    std::size_t MqttClientConnection::MqttFormatter::EncodedLength(infra::BoundedConstString value)
    {
        return 2 + value.size();
    }

    void MqttClientConnection::MqttFormatter::AddString(infra::BoundedConstString value)
    {
        stream << BigEndianUint16(static_cast<uint16_t>(value.size())) << infra::StringAsByteRange(value);
    }

    void MqttClientConnection::MqttFormatter::Header(PacketType packetType, std::size_t size, uint8_t flags)
    {
        stream << MakePacketType(packetType, flags);

        while (size > 127)
        {
            stream << static_cast<uint8_t>((size & 0x7f) | 0x80);
            size >>= 8;
        }
        stream << static_cast<uint8_t>(size);
    }

    uint8_t MqttClientConnection::MqttFormatter::MakePacketType(PacketType packetType, uint8_t flags)
    {
        return static_cast<uint8_t>((static_cast<uint8_t>(packetType) << 4) | flags);
    }

    MqttClientConnection::MqttParser::MqttParser(infra::DataInputStream stream)
        : stream(stream)
    {
        ExtractType();
        ExtractSize();
    }

    MqttClientConnection::PacketType MqttClientConnection::MqttParser::GetPacketType() const
    {
        return packetType;
    }

    void MqttClientConnection::MqttParser::ExtractType()
    {
        uint8_t combinedPacketType;
        this->stream >> combinedPacketType;
        packetType = static_cast<PacketType>(combinedPacketType >> 4);
    }

    void MqttClientConnection::MqttParser::ExtractSize()
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

    MqttClientConnection::MqttClientConnection(MqttClientFactory& factory, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
        : state(infra::InPlaceType<StateConnecting>(), *this, factory, clientId, username, password)
    {}

    void MqttClientConnection::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        state->SendStreamAvailable(std::move(stream));
    }

    void MqttClientConnection::DataReceived()
    {
        state->DataReceived(*ConnectionObserver::Subject().ReceiveStream());
    }

    void MqttClientConnection::Connected()
    {
        state->Connected();
    }

    MqttClientConnection::StateBase::StateBase(MqttClientConnection& clientConnection)
        : clientConnection(clientConnection)
    {}

    void MqttClientConnection::StateBase::Connected()
    {
        std::abort();
    }

    MqttClientConnection::StateConnecting::StateConnecting(MqttClientConnection& clientConnection, MqttClientFactory& factory, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
        : StateBase(clientConnection)
        , factory(factory)
        , clientId(clientId)
        , username(username)
        , password(password)
        , timeout(std::chrono::minutes(1), [this]() { Timeout(); })
    {}

    void MqttClientConnection::StateConnecting::Connected()
    {
        clientConnection.ConnectionObserver::Subject().RequestSendStream(MqttFormatter::MessageSize<PacketType::packetTypeConnect>(clientId, username, password));
    }

    void MqttClientConnection::StateConnecting::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        MqttFormatter formatter(*stream);
        formatter.Message<PacketType::packetTypeConnect>(clientId, username, password);
    }

    void MqttClientConnection::StateConnecting::DataReceived(infra::DataInputStream stream)
    {
        MqttParser parser(stream);
        if (!stream.Failed())
        {
            if (parser.GetPacketType() == PacketType::packetTypeConAck)
                factory.ConnectionEstablished([this](infra::SharedPtr<MqttClient> client)
                {
                    clientConnection.client = client;
                });
            else
            {
                factory.ConnectionFailed(MqttClientFactory::ConnectFailReason::initializationFailed);
                clientConnection.ConnectionObserver::Subject().AbortAndDestroy();
            }
        }
    }

    void MqttClientConnection::StateConnecting::Timeout()
    {
        factory.ConnectionFailed(MqttClientFactory::ConnectFailReason::initializationTimedOut);
        clientConnection.ConnectionObserver::Subject().AbortAndDestroy();
    }

    MqttClientConnector::MqttClientConnector(MqttClientFactory& factory, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password)
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
                factory.ConnectionFailed(MqttClientFactory::ConnectFailReason::refused);
                break;
            case ConnectFailReason::connectionAllocationFailed:
                factory.ConnectionFailed(MqttClientFactory::ConnectFailReason::connectionAllocationFailed);
                break;
            default:
                std::abort();
        }
    }
}
