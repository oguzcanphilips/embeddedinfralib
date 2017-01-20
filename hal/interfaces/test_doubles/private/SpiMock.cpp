#include "infra/event/public/EventDispatcher.hpp"
#include "hal/interfaces/test_doubles/public/SpiMock.hpp"

namespace hal
{
    void SpiMock::SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart)
    {
        actionOnStart();

        if (!sendData.empty())
            SendDataMock(std::vector<uint8_t>(sendData.begin(), sendData.end()), nextAction);
        if (!receiveData.empty())
        {
            std::vector<uint8_t> dataToBeReceived = ReceiveDataMock(nextAction);
            EXPECT_EQ(dataToBeReceived.size(), receiveData.size());                                                     //TICS !CFL#001
            std::copy(dataToBeReceived.begin(), dataToBeReceived.end(), receiveData.begin());
        }

        infra::EventDispatcher::Instance().Schedule(actionOnCompletion);
    }

    void SpiAsynchronousMock::SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart)
    {
        this->actionOnStart = actionOnStart;
        this->actionOnCompletion = actionOnCompletion;

        std::pair<bool, std::vector<uint8_t>> result = SendAndReceiveMock(std::vector<uint8_t>(sendData.begin(), sendData.end()), nextAction);
        if (result.first)
        {
            EXPECT_EQ(receiveData.size(), result.second.size());                                                        //TICS !CFL#001
            std::copy(result.second.begin(), result.second.end(), receiveData.begin());

            actionOnStart();
            if (scheduleActionCompleteAutomatically)
                infra::EventDispatcher::Instance().Schedule(actionOnCompletion);
        }
    }
}
