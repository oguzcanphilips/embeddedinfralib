#ifndef HAL_SPI_MOCK_HPP
#define HAL_SPI_MOCK_HPP

#include "gmock/gmock.h"
#include "hal/interfaces/public/Spi.hpp"
#include "infra/util/public/AutoResetFunction.hpp"
#include <cstdint>
#include <vector>
#include <utility>

namespace hal
{
    //TICS -INT#002: A mock or stub may have public data
    class SpiMock
        : public SpiMaster
    {
    public:
        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart) override;
        MOCK_METHOD1(SetCommunicationConfigurator, void(CommunicationConfigurator& configurator));
        MOCK_METHOD0(ResetCommunicationConfigurator, void());

        MOCK_METHOD2(SendDataMock, void(std::vector<uint8_t> dataSent, SpiAction nextAction));
        MOCK_METHOD1(ReceiveDataMock, std::vector<uint8_t>(SpiAction nextAction));
    };

    class SpiAsynchronousMock
        : public SpiMaster
    {
    public:
        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart) override;

        infra::Function<void()> actionOnCompletion;
        infra::Function<void()> actionOnStart;

        using SendAndReceiveResult = std::pair<bool, std::vector<uint8_t>>;
        MOCK_METHOD2(SendAndReceiveMock, SendAndReceiveResult(std::vector<uint8_t> dataSent, SpiAction nextAction));
        MOCK_METHOD1(SetCommunicationConfigurator, void(CommunicationConfigurator& configurator));
        MOCK_METHOD0(ResetCommunicationConfigurator, void());

        bool scheduleActionCompleteAutomatically = true;
    };
}

#endif
