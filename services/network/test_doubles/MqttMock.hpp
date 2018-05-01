#ifndef MULTICAST_MOCK_HPP
#define MULTICAST_MOCK_HPP

#include "gmock/gmock.h"
#include "services/network/Mqtt.hpp"

namespace services
{
    class MqttClientFactoryMock
        : public MqttClientFactory
    {
    public:
        MOCK_METHOD1(ConnectionEstablished, void(infra::AutoResetFunction<void(infra::SharedPtr<MqttClient> client)>&& createdClient));
        MOCK_METHOD1(ConnectionFailed, void(ConnectFailReason reason));
    };
}
#endif
