#ifndef HAL_SERIAL_COMMUNICATION_MOCK_HPP
#define HAL_SERIAL_COMMUNICATION_MOCK_HPP

#include "gmock/gmock.h"
#include "hal/interfaces/public/SerialCommunication.hpp"

namespace hal
{
    //TICS -INT#002: A mock or stub may have public data
    class SerialCommunicationMock
        : public SerialCommunication
    {
    public:
        // SerialCommunication Interface
        virtual void SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion) override;
        virtual void ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived) override;

        MOCK_METHOD1(SendDataMock, void(std::vector<uint8_t>));

        infra::Function<void()> actionOnCompletion;
        infra::Function<void(infra::ConstByteRange data)> dataReceived;
    };
}

#endif
