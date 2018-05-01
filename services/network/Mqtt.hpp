#ifndef SERVICES_MQTT_HPP
#define SERVICES_MQTT_HPP

#include "infra/timer/Timer.hpp"
#include "infra/util/PolymorphicVariant.hpp"
#include "infra/util/SharedOptional.hpp"
#include "services/network/Connection.hpp"

namespace services
{
    class MqttClientConnection;

    class MqttClient
        : public infra::SingleObserver<MqttClient, MqttClientConnection>
    {
    public:
    };

    class MqttClientFactory
    {
    protected:
        MqttClientFactory() = default;
        MqttClientFactory(const MqttClientFactory& other) = delete;
        MqttClientFactory& operator=(const MqttClientFactory& other) = delete;
        ~MqttClientFactory() = default;

    public:
        enum ConnectFailReason
        {
            refused,
            connectionAllocationFailed,
            initializationFailed,
            initializationTimedOut
        };

        virtual void ConnectionEstablished(infra::AutoResetFunction<void(infra::SharedPtr<MqttClient> client)>&& createdClient) = 0;
        virtual void ConnectionFailed(ConnectFailReason reason) = 0;
    };

    class MqttClientConnection
        : public ConnectionObserver
        , public infra::Subject<MqttClient>
    {
    public:
        MqttClientConnection(MqttClientFactory& factory, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password);

        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) override;
        virtual void DataReceived() override;
        virtual void Connected() override;

    private:
        enum class PacketType : uint8_t
        {
            packetTypeConnect = 1,
            packetTypeConAck = 2,
            packetTypePublish = 3,
            packetTypePubAck = 4,
            packetTypePubRec = 5,
            packetTypePubRel = 6,
            packetTypePubComp = 7,
            packetTypeSubscribe = 8,
            packetTypeSubAck = 9,
            packetTypeUnsubscribe = 10,
            packetTypeUnsubAck = 11,
            packetTypePingReq = 12,
            packetTypePingResp = 13,
            packetTypeDisconnect = 14
        };

        class MqttFormatter
        {
        private:
            template<PacketType packetType>
                struct InPlaceType {};

        public:
            MqttFormatter(infra::DataOutputStream stream);

            template<PacketType packetType, class... Args>
                void Message(Args&&... args);

            template<PacketType packetType, class... Args>
                static std::size_t MessageSize(Args&&... args);

        private:
            void MessageImpl(InPlaceType<PacketType::packetTypeConnect>, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password);
            static std::size_t MessageSizeImpl(InPlaceType<PacketType::packetTypeConnect>, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password);
            static std::size_t EncodedLength(infra::BoundedConstString value);
            void AddString(infra::BoundedConstString value);
            void Header(PacketType packetType, std::size_t size, uint8_t flags = 0);
            uint8_t MakePacketType(PacketType packetType, uint8_t flags);

        private:
            infra::DataOutputStream stream;
        };

        class MqttParser
        {
        public:
            MqttParser(infra::DataInputStream stream);

            PacketType GetPacketType() const;

        private:
            void ExtractType();
            void ExtractSize();

        private:
            infra::DataInputStream stream;
            PacketType packetType;
            uint32_t size = 0;
        };

        struct BigEndianUint16
        {
            BigEndianUint16() = default;
            BigEndianUint16(uint16_t value)
                : msb(static_cast<uint8_t>(value >> 8))
                , lsb(static_cast<uint8_t>(value))
            {}

            uint8_t msb = 0;
            uint8_t lsb = 0;
        };

        struct PacketConnect
        {
            BigEndianUint16  protocolNameLength = 4;
            std::array<char, 4> protocolName{{ 'M', 'Q', 'T', 'T' }};
            uint8_t protocolLevel = 4;
            uint8_t connectFlags = 0xc1;    // Username, password, clean session, no will
            BigEndianUint16 keepAliveMsb = 0;
        };

        class StateBase
        {
        public:
            StateBase(MqttClientConnection& clientConnection);
            StateBase(const StateBase& other) = delete;
            StateBase& operator=(const StateBase& other) = delete;
            virtual ~StateBase() = default;

        public:
            virtual void Connected();
            virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) = 0;
            virtual void DataReceived(infra::DataInputStream stream) = 0;

        protected:
            MqttClientConnection& clientConnection;
        };

        class StateConnecting
            : public StateBase
        {
        public:
            StateConnecting(MqttClientConnection& clientConnection, MqttClientFactory& factory, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password);

            virtual void Connected() override;
            virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) override;
            virtual void DataReceived(infra::DataInputStream stream) override;

        private:
            void Timeout();

        private:
            MqttClientFactory& factory;
            infra::BoundedConstString clientId;
            infra::BoundedConstString username;
            infra::BoundedConstString password;
            infra::TimerSingleShot timeout;
        };

        class StateConnected
            : public StateBase
        {
        public:
            using StateBase::StateBase;

            virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) override;
            virtual void DataReceived(infra::DataInputStream stream) override;
        };

    private:
        infra::PolymorphicVariant<StateBase, StateConnecting, StateConnected> state;
        infra::SharedPtr<MqttClient> client;
    };

    class MqttClientConnector
        : public ClientConnectionObserverFactory
    {
    public:
        MqttClientConnector(MqttClientFactory& factory, infra::BoundedConstString clientId, infra::BoundedConstString username, infra::BoundedConstString password);

        virtual void ConnectionEstablished(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver) override;
        virtual void ConnectionFailed(ConnectFailReason reason) override;

    private:
        MqttClientFactory& factory;
        infra::SharedOptional<MqttClientConnection> connection;
        infra::BoundedConstString clientId;
        infra::BoundedConstString username;
        infra::BoundedConstString password;
    };
}

#endif
