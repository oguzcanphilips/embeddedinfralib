#ifndef HAL_I2C_MOCK_HPP
#define HAL_I2C_MOCK_HPP

#include "gmock/gmock.h"
#include "hal/interfaces/public/I2c.hpp"

namespace hal
{
    class I2cMasterMock
        : public hal::I2cMaster
    {
    public:
        virtual void SendData(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction,
            infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent) override;
        virtual void ReceiveData(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction,
            infra::Function<void(hal::Result)> onReceived) override;

        MOCK_METHOD3(SendDataMock, void(hal::I2cAddress address, hal::Action nextAction, std::vector<uint8_t> data));
        MOCK_METHOD2(ReceiveDataMock, std::vector<uint8_t>(hal::I2cAddress address, hal::Action nextAction));
    };

    class I2cMasterMockWithoutAutomaticDone
        : public hal::I2cMaster
    {
    public:
        virtual void SendData(hal::I2cAddress address, infra::ConstByteRange data, hal::Action nextAction,
            infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent) override;
        virtual void ReceiveData(hal::I2cAddress address, infra::ByteRange data, hal::Action nextAction,
            infra::Function<void(hal::Result)> onReceived) override;

        MOCK_METHOD3(SendDataMock, void(hal::I2cAddress address, hal::Action nextAction, std::vector<uint8_t> data));
        MOCK_METHOD2(ReceiveDataMock, std::vector<uint8_t>(hal::I2cAddress address, hal::Action nextAction));

        infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent;
        infra::Function<void(hal::Result)> onReceived;
    };
}

#endif
