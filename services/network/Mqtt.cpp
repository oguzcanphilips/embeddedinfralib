#include "services/network/Mqtt.hpp"

namespace services
{
    class MqttClientConnection::MqttParser
    {
    public:
        MqttParser(infra::DataInputStream stream)
            : stream(stream)
        {
            uint8_t combinedPacketType;
            this->stream >> combinedPacketType;
            packetType = static_cast<PacketType>(combinedPacketType >> 4);

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

        PacketType GetPacketType() const
        {
            return packetType;
        };

    private:
        infra::DataInputStream stream;
        PacketType packetType;
        uint32_t size = 0;
    };

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

    uint8_t MqttClientConnection::MakePacketType(PacketType packetType, uint8_t flags)
    {
        return static_cast<uint8_t>((static_cast<uint8_t>(packetType) << 4) | flags);
    }

    template<class T>
    void MqttClientConnection::StreamHeader(infra::DataOutputStream stream, uint8_t packetType, T packet, std::size_t payloadSize)
    {
        stream << packetType;

        std::size_t size = sizeof(packet) + payloadSize;
        while (size > 127)
        {
            stream << static_cast<uint8_t>((size & 0x7f) | 0x80);
            size >>= 8;
        }
        stream << static_cast<uint8_t>(size);

        stream << packet;
    }

    void MqttClientConnection::StreamString(infra::DataOutputStream stream, infra::BoundedConstString value)
    {
        stream << BigEndianUint16(static_cast<uint16_t>(value.size())) << infra::StringAsByteRange(value);
    }

    std::size_t MqttClientConnection::EncodedLength(infra::BoundedConstString value)
    {
        return 2 + value.size();
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
        clientConnection.ConnectionObserver::Subject().RequestSendStream(5 + sizeof(PacketConnect) + EncodedLength(clientId) + EncodedLength(username) + EncodedLength(password));
    }

    void MqttClientConnection::StateConnecting::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        StreamHeader(*stream, MakePacketType(PacketType::packetTypeConnect), PacketConnect{}, EncodedLength(clientId) + EncodedLength(username) + EncodedLength(password));
        StreamString(*stream, clientId);
        StreamString(*stream, username);
        StreamString(*stream, password);
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
