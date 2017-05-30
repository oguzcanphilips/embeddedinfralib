#ifndef HAL_I2C_MOCK_HPP
#define HAL_I2C_MOCK_HPP

#include "gmock/gmock.h"
#include "hal/interfaces/I2c.hpp"

namespace hal
{
    //TICS -INT#002: A mock or stub may have public data
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

    //TICS -INT#002: A mock or stub may have public data
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

    //TICS -INT#002: A mock or stub may have public data
    class I2cSlaveMock
        : public hal::I2cSlave
    {
    public:
        virtual void AddSlave(uint8_t ownAddress, infra::Function<void(DataDirection)> onAddressed) override;
        virtual void SendData(infra::ConstByteRange data, 
            infra::Function<void(Result, uint32_t numberOfBytesSent)> onSent) override;
        virtual void ReceiveData(infra::ByteRange data, bool lastOfSession, 
            infra::Function<void(Result, uint32_t numberOfBytesReceived)> onReceived) override;

        MOCK_METHOD1(AddSlaveMock, void(uint8_t address));
        MOCK_METHOD1(SendDataMock, void(infra::ConstByteRange data));
        MOCK_METHOD2(ReceiveDataMock, void(infra::ByteRange data, bool lastOfSession));

        MOCK_METHOD1(RemoveSlave, void(uint8_t address));
        MOCK_METHOD0(StopTransceiving, void());
    };

    //TICS -INT#002: A mock or stub may have public data
    class I2cSlaveMockWithManualCallback
        : public hal::I2cSlaveMock
    {
    public:
        virtual void AddSlave(uint8_t ownAddress, infra::Function<void(DataDirection)> onAddressed) override;
        virtual void SendData(infra::ConstByteRange data,
            infra::Function<void(Result, uint32_t numberOfBytesSent)> onSent) override;
        virtual void ReceiveData(infra::ByteRange data, bool lastOfSession,
            infra::Function<void(Result, uint32_t numberOfBytesReceived)> onReceived) override;

        infra::Function<void(hal::DataDirection)> onAddressed;
        infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onSent;
        infra::Function<void(hal::Result, uint32_t numberOfBytesSent)> onReceived;
    };
}

#endif
