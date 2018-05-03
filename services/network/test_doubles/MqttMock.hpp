#ifndef MULTICAST_MOCK_HPP
#define MULTICAST_MOCK_HPP

#include "gmock/gmock.h"
#include "services/network/Mqtt.hpp"

namespace services
{
    class MqttClientObserverMock
        : public MqttClientObserver
    {
    public:
        MOCK_METHOD0(Connected, void());
        MOCK_METHOD0(PublishDone, void());
    };

    class MqttClientObserverFactoryMock
        : public MqttClientObserverFactory
    {
    public:
        MOCK_METHOD1(ConnectionEstablished, void(infra::AutoResetFunction<void(infra::SharedPtr<MqttClientObserver> client)>&& createdClientObserver));
        MOCK_METHOD1(ConnectionFailed, void(ConnectFailReason reason));
    };
}
#endif
