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
            EXPECT_EQ(dataToBeReceived.size(), receiveData.size());
            std::copy(dataToBeReceived.begin(), dataToBeReceived.end(), receiveData.begin());
        }

        infra::EventDispatcher::Instance().Schedule(actionOnCompletion);
    }

    uint32_t SpiMock::Speed() const
    {
        return speed;
    }

    void SpiMock::ConfigSpeed(uint32_t speedInkHz)
    {
        ConfigSpeedMock(speedInkHz);
    }

    uint8_t SpiMock::Mode() const
    {
        return spiMode;
    }

    void SpiMock::ConfigMode(uint8_t spiMode)
    {
        ConfigModeMock(spiMode);
    }

    void SpiAsynchronousMock::SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart)
    {
        this->actionOnStart = actionOnStart;
        this->actionOnCompletion = actionOnCompletion;

        std::pair<bool, std::vector<uint8_t>> result = SendAndReceiveMock(std::vector<uint8_t>(sendData.begin(), sendData.end()), nextAction);
        if (result.first)
        {
            EXPECT_EQ(receiveData.size(), result.second.size());
            std::copy(result.second.begin(), result.second.end(), receiveData.begin());

            actionOnStart();
            infra::EventDispatcher::Instance().Schedule(actionOnCompletion);
        }
    }

    uint32_t SpiAsynchronousMock::Speed() const
    {
        return speed;
    }

    void SpiAsynchronousMock::ConfigSpeed(uint32_t speedInkHz)
    {
        ConfigSpeedMock(speedInkHz);
    }

    uint8_t SpiAsynchronousMock::Mode() const
    {
        return spiMode;
    }

    void SpiAsynchronousMock::ConfigMode(uint8_t spiMode)
    {
        ConfigModeMock(spiMode);
    }
}
