#ifndef HAL_I2C_HPP
#define HAL_I2C_HPP

#include "infra/util/public/Function.hpp"
#include "infra/util/public/ByteRange.hpp"

namespace hal
{
    enum class Result
    {
        complete,
        partialComplete,
        busError
    };

    enum class Action
    {
        continueSession,
        repeatedStart,
        stop
    };

    enum class DataDirection
    {
        send,
        receive
    };

    class I2cAddress
    {
    public:
        explicit I2cAddress(uint16_t address);

        bool operator==(const I2cAddress& other) const;

        uint16_t address;
    };

    class I2cMaster
    {
    public:
        virtual void SendData(I2cAddress address, infra::ConstByteRange data, Action nextAction, infra::Function<void(Result, uint32_t numberOfBytesSent)> onSent) = 0;

        virtual void ReceiveData(I2cAddress address, infra::ByteRange data, Action nextAction, infra::Function<void(Result)> onReceived) = 0;
    };

    class I2cSlave
    {
    public:
        virtual void AddSlave(uint8_t ownAddress, infra::Function<void(DataDirection)> onAddressed) = 0;
        virtual void RemoveSlave(uint8_t ownAddress) = 0;

        virtual void SendData(infra::ConstByteRange data, infra::Function<void(Result, uint32_t numberOfBytesSent)> onSent) = 0;
        virtual void ReceiveData(infra::ByteRange data, bool lastOfSession, infra::Function<void(Result, uint32_t numberOfBytesReceived)> onReceived) = 0;

        virtual void StopTransceiving() = 0;
    };
}

#endif
