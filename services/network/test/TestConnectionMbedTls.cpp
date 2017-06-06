#include "gmock/gmock.h"
#include "hal/windows/SynchronousRandomDataGeneratorWin.hpp"
#include "infra/event/test_helper/EventDispatcherWithWeakPtrFixture.hpp"
#include "infra/util/test_helper/MockHelpers.hpp"
#include "services/network/ConnectionMbedTls.hpp"
#include "services/network/test_doubles/ConnectionLoopBack.hpp"
#include "services/network/test_doubles/ConnectionMock.hpp"
#include "services/network/test_doubles/ConnectionStub.hpp"

class ConnectionMbedTlsTest
    : public testing::Test
    , public infra::EventDispatcherWithWeakPtrFixture
{
public:
    ConnectionMbedTlsTest()
        : connectionFactory(network, randomDataGenerator)
        , thisListener(infra::UnOwnedSharedPtr(*this))
    {}

    services::ZeroCopyServerConnectionObserverFactoryMock serverObserverFactory;
    services::ZeroCopyClientConnectionObserverFactoryMock clientObserverFactory;
    testing::StrictMock<services::ZeroCopyConnectionFactoryMock> network;
    services::ConnectionLoopBackFactory loopBackNetwork;
    hal::SynchronousRandomDataGeneratorWin randomDataGenerator;
    infra::SharedPtr<void> thisListener;
    services::ZeroCopyServerConnectionObserverFactory* mbedTlsObserverFactory;
    services::ZeroCopyConnection* mbedTlsConnection;
    services::ConnectionFactoryMbedTls::WithMaxConnectionsListenersAndConnectors<2, 1, 1> connectionFactory;
};

TEST_F(ConnectionMbedTlsTest, when_allocation_on_network_fails_Listen_returns_nullptr)
{
    EXPECT_CALL(network, Listen(1234, testing::_)).WillOnce(testing::Return(nullptr));
    infra::SharedPtr<void> listener = connectionFactory.Listen(1234, serverObserverFactory);
    EXPECT_EQ(nullptr, listener);
}

TEST_F(ConnectionMbedTlsTest, when_listener_allocation_fails_Listen_returns_nullptr)
{
    EXPECT_CALL(network, Listen(1234, testing::_)).WillOnce(testing::Return(thisListener));
    infra::SharedPtr<void> listener1 = connectionFactory.Listen(1234, serverObserverFactory);
    infra::SharedPtr<void> listener2 = connectionFactory.Listen(1234, serverObserverFactory);
    EXPECT_EQ(nullptr, listener2);
}

TEST_F(ConnectionMbedTlsTest, Listen_returns_listener)
{
    EXPECT_CALL(network, Listen(1234, testing::_)).WillOnce(testing::Return(thisListener));
    infra::SharedPtr<void> listener = connectionFactory.Listen(1234, serverObserverFactory);
    EXPECT_NE(nullptr, listener);
}

TEST_F(ConnectionMbedTlsTest, create_connection)
{
    services::ConnectionFactoryMbedTls::WithMaxConnectionsListenersAndConnectors<2, 1, 1> tlsNetwork(loopBackNetwork, randomDataGenerator);
    infra::SharedPtr<void> listener = tlsNetwork.Listen(1234, serverObserverFactory);
    infra::SharedPtr<void> connector = tlsNetwork.Connect(services::IPv4Address(), 1234, clientObserverFactory);
    
    infra::SharedOptional<services::ZeroCopyConnectionObserverMock> observer1;
    infra::SharedOptional<services::ZeroCopyConnectionObserverMock> observer2;
    EXPECT_CALL(serverObserverFactory, ConnectionAccepted(testing::_))
        .WillOnce(infra::Lambda([&](services::ZeroCopyConnection& connection) { return observer1.Emplace(connection); }));
    EXPECT_CALL(clientObserverFactory, ConnectionEstablished(testing::_))
        .WillOnce(infra::Lambda([&](services::ZeroCopyConnection& connection) { return observer2.Emplace(connection); }));
    ExecuteAllActions();
    observer1->Subject().AbortAndDestroy();
}

TEST_F(ConnectionMbedTlsTest, send_and_receive_data)
{
    services::ConnectionFactoryMbedTls::WithMaxConnectionsListenersAndConnectors<2, 1, 1> tlsNetwork(loopBackNetwork, randomDataGenerator);
    infra::SharedPtr<void> listener = tlsNetwork.Listen(1234, serverObserverFactory);
    infra::SharedPtr<void> connector = tlsNetwork.Connect(services::IPv4Address(), 1234, clientObserverFactory);

    infra::SharedOptional<services::ZeroCopyConnectionObserverStub> observer1;
    infra::SharedOptional<services::ZeroCopyConnectionObserverStub> observer2;
    EXPECT_CALL(serverObserverFactory, ConnectionAccepted(testing::_))
        .WillOnce(infra::Lambda([&](services::ZeroCopyConnection& connection) { return observer1.Emplace(connection); }));
    EXPECT_CALL(clientObserverFactory, ConnectionEstablished(testing::_))
        .WillOnce(infra::Lambda([&](services::ZeroCopyConnection& connection) { return observer2.Emplace(connection); }));
    ExecuteAllActions();

    observer2->SendData(std::vector<uint8_t>{ 1, 2, 3, 4 });
    ExecuteAllActions();
    EXPECT_EQ((std::vector<uint8_t>{ 1, 2, 3, 4 }), observer1->receivedData);

    observer1->SendData(std::vector<uint8_t>{ 5, 6, 7, 8 });
    ExecuteAllActions();
    EXPECT_EQ((std::vector<uint8_t>{ 5, 6, 7, 8 }), observer2->receivedData);

    observer1->Subject().AbortAndDestroy();
}
