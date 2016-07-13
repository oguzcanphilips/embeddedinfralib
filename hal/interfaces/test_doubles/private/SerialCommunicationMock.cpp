#include "hal/interfaces/test_doubles/public/SerialCommunicationMock.hpp"

namespace hal
{
    void SerialCommunicationMock::SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion)
    {
        SendDataMock(std::vector<uint8_t>(data.begin(), data.end()));

        this->actionOnCompletion = actionOnCompletion;
    }

    void SerialCommunicationMock::ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived)
    {
        this->dataReceived = dataReceived;
    }
}
