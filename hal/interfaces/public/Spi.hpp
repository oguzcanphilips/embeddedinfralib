#ifndef HAL_SPI_HPP
#define HAL_SPI_HPP

#include "hal/interfaces/public/CommunicationConfigurator.hpp"
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
    protected:
        SpiMaster() = default;
        SpiMaster(const SpiMaster& other) = delete;
        SpiMaster& operator=(const SpiMaster& other) = delete;
        ~SpiMaster() = default;

    public:
        void SendData(infra::ConstByteRange data, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion);
        void ReceiveData(infra::ByteRange data, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion);
        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart = infra::emptyFunction) = 0;
        virtual void SetCommunicationConfigurator(CommunicationConfigurator& configurator) = 0;
        virtual void ResetCommunicationConfigurator() = 0;
    };
}

#endif
