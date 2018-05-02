#include "gmock/gmock.h"
#include "infra/timer/test_helper/ClockFixture.hpp"
#include "infra/util/test_helper/MockHelpers.hpp"
#include "services/network/Mqtt.hpp"
#include "services/network/test_doubles/ConnectionStub.hpp"
#include "services/network/test_doubles/MqttMock.hpp"

class MqttTest
    : public testing::Test
    , public infra::ClockFixture
{
public:
    MqttTest()
        : connector(factory, "clientId", "username", "password")
        , connectionPtr(infra::UnOwnedSharedPtr(connection))
        , clientPtr(infra::UnOwnedSharedPtr(client))
    {}

    void Connect()
    {
        connector.ConnectionEstablished([this](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
        {
            connectionObserver->Attach(connection);
            connection.SetOwnership(nullptr, connectionObserver);
            connectionObserver->Connected();
        });

        ExecuteAllActions();

        EXPECT_CALL(factory, ConnectionEstablished(testing::_)).WillOnce(infra::Lambda([this](infra::AutoResetFunction<void(infra::SharedPtr<services::MqttClient> client)>& createdClient)
        {
            createdClient(clientPtr);
        }));
        connection.SimulateDataReceived(std::vector<uint8_t>{ 0x20, 0x00 });
        ExecuteAllActions();

        connection.sentData.clear();
    }

    testing::StrictMock<services::MqttClientFactoryMock> factory;
    services::MqttClientConnector connector;
    testing::StrictMock<services::ConnectionStub> connection;
    infra::SharedPtr<services::Connection> connectionPtr;
    testing::StrictMock<services::MqttClientMock> client;
    infra::SharedPtr<services::MqttClient> clientPtr;
};

TEST_F(MqttTest, refused_connection_propagates_to_MqttClientFactory)
{
    EXPECT_CALL(factory, ConnectionFailed(services::MqttClientFactory::ConnectFailReason::refused));
    connector.ConnectionFailed(services::ClientConnectionObserverFactory::ConnectFailReason::refused);
}

TEST_F(MqttTest, connection_failed_propagates_to_MqttClientFactory)
{
    EXPECT_CALL(factory, ConnectionFailed(services::MqttClientFactory::ConnectFailReason::connectionAllocationFailed));
    connector.ConnectionFailed(services::ClientConnectionObserverFactory::ConnectFailReason::connectionAllocationFailed);
}

TEST_F(MqttTest, after_connected_connect_message_is_sent)
{
    connector.ConnectionEstablished([this](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
    {
        connectionObserver->Attach(connection);
        connection.SetOwnership(nullptr, connectionObserver);
        connectionObserver->Connected();
    });

    ExecuteAllActions();

    EXPECT_EQ((std::vector<uint8_t>{ 0x10, 0x28, 0x00, 0x04, 'M' , 'Q' , 'T' , 'T' , 0x04, 0xc1, 0x00, 0x00, 0x00, 0x08, 'c' , 'l' ,
        'i' , 'e' , 'n' , 't' , 'I' , 'd' , 0x00, 0x08, 'u' , 's' , 'e' , 'r' , 'n' , 'a' , 'm' , 'e' ,
        0x00, 0x08, 'p' , 'a' , 's' , 's' , 'w' , 'o' , 'r' , 'd' }), connection.sentData);
}

TEST_F(MqttTest, after_conack_MqttClient_is_connected)
{
    connector.ConnectionEstablished([this](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
    {
        connectionObserver->Attach(connection);
        connection.SetOwnership(nullptr, connectionObserver);
        connectionObserver->Connected();
    });

    ExecuteAllActions();

    EXPECT_CALL(factory, ConnectionEstablished(testing::_));
    connection.SimulateDataReceived(std::vector<uint8_t>{ 0x20, 0x00 });
    ExecuteAllActions();
}

TEST_F(MqttTest, partial_conack_is_ignored)
{
    connector.ConnectionEstablished([this](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
    {
        connectionObserver->Attach(connection);
        connection.SetOwnership(nullptr, connectionObserver);
        connectionObserver->Connected();
    });

    ExecuteAllActions();

    connection.SimulateDataReceived(std::vector<uint8_t>{ 0x20 });
    ExecuteAllActions();
}

TEST_F(MqttTest, invalid_response_results_in_ConnectionFailed)
{
    connector.ConnectionEstablished([this](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
    {
        connectionObserver->Attach(connection);
        connection.SetOwnership(nullptr, connectionObserver);
        connectionObserver->Connected();
    });

    ExecuteAllActions();

    EXPECT_CALL(factory, ConnectionFailed(services::MqttClientFactory::ConnectFailReason::initializationFailed));
    EXPECT_CALL(connection, AbortAndDestroyMock());
    connection.SimulateDataReceived(std::vector<uint8_t>{ 0x30, 0 });
    ExecuteAllActions();
}

TEST_F(MqttTest, timeout_results_in_ConnectionFailed)
{
    connector.ConnectionEstablished([this](infra::SharedPtr<services::ConnectionObserver> connectionObserver)
    {
        connectionObserver->Attach(connection);
        connection.SetOwnership(nullptr, connectionObserver);
        connectionObserver->Connected();
    });

    ExecuteAllActions();

    EXPECT_CALL(factory, ConnectionFailed(services::MqttClientFactory::ConnectFailReason::initializationTimedOut));
    EXPECT_CALL(connection, AbortAndDestroyMock());
    ForwardTime(std::chrono::minutes(1));
}

TEST_F(MqttTest, Publish_some_data)
{
    Connect();

    client.Subject().Publish("topic", "payload");

    EXPECT_CALL(client, PublishDone());
    ExecuteAllActions();

    EXPECT_EQ((std::vector<uint8_t>{ 0x30, 0x10, 0x00, 0x05, 't', 'o', 'p', 'i',
        'c', 'p', 'a', 'y', 'l', 'o', 'a', 'd' }), connection.sentData);
}
