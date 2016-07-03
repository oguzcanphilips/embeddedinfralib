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
    class SpiMock
        : public SpiMaster
    {
    public:
        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart) override;
        virtual uint32_t Speed() const override;
        virtual void ConfigSpeed(uint32_t speedInkHz) override;
        virtual uint8_t Mode() const override;
        virtual void ConfigMode(uint8_t spiMode) override;

        MOCK_METHOD2(SendDataMock, void(std::vector<uint8_t> dataSent, SpiAction nextAction));
        MOCK_METHOD1(ReceiveDataMock, std::vector<uint8_t>(SpiAction nextAction));
        MOCK_CONST_METHOD0(SpeedMock, uint32_t());
        MOCK_METHOD1(ConfigSpeedMock, void(uint32_t speedInkHz));
        MOCK_CONST_METHOD0(ModeMock, uint8_t());
        MOCK_METHOD1(ConfigModeMock, void(uint8_t spiMode));

        uint32_t speed = 100;
        uint8_t spiMode = 0;
    };

    class SpiAsynchronousMock
        : public SpiMaster
    {
    public:
        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart) override;
        virtual uint32_t Speed() const override;
        virtual void ConfigSpeed(uint32_t speedInkHz) override;
        virtual uint8_t Mode() const override;
        virtual void ConfigMode(uint8_t spiMode) override;

        infra::Function<void()> actionOnCompletion;
        infra::Function<void()> actionOnStart;

        using SendAndReceiveResult = std::pair<bool, std::vector<uint8_t>>;
        MOCK_METHOD2(SendAndReceiveMock, SendAndReceiveResult(std::vector<uint8_t> dataSent, SpiAction nextAction));
        MOCK_METHOD1(ConfigSpeedMock, void(uint32_t speedInkHz));
        MOCK_METHOD1(ConfigModeMock, void(uint8_t spiMode));

        uint32_t speed = 100;
        uint8_t spiMode = 0;
    };
}

#endif
