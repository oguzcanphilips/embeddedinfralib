#include "hal/interfaces/test_doubles/public/SerialCommunicationStub.hpp"

namespace hal
{
    SerialCommunicationStub::SerialCommunicationStub()
    {}

    void SerialCommunicationStub::SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion)
    {
        for (size_t i = 0; i < data.size(); ++i)
            dataReceivedByStub.push_back(data[i]);

        actionOnCompletion();
    }

    void SerialCommunicationStub::ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived)
    {
        this->dataReceived = dataReceived;
    }
}