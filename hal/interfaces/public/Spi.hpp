#ifndef HAL_SPI_HPP
#define HAL_SPI_HPP

#include "infra/util/public/AutoResetFunction.hpp"
#include "infra/util/public/ByteRange.hpp"

namespace hal
{
    enum SpiAction
    {
        continueSession,
        stop
    };

    class SpiMaster
    {
    public:
        SpiMaster() = default;
        SpiMaster(const SpiMaster& other) = delete;
        SpiMaster& operator=(const SpiMaster& other) = delete;

    protected:
        ~SpiMaster() = default;

    public:
        void SendData(infra::ConstByteRange data, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion);
        void ReceiveData(infra::ByteRange data, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion);
        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart = infra::emptyFunction) = 0;

        virtual uint32_t Speed() const = 0;
        virtual void ConfigSpeed(uint32_t speedInkHz) = 0;
        virtual uint8_t Mode() const = 0;
        virtual void ConfigMode(uint8_t spiMode) = 0;   // 0..3
    };
}

#endif
