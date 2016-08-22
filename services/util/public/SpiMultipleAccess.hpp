#ifndef SERVICES_SPI_MULTIPLE_ACCESS_HPP
#define SERVICES_SPI_MULTIPLE_ACCESS_HPP

#include "hal/interfaces/public/Spi.hpp"
#include "infra/event/public/ClaimableResource.hpp"
#include "infra/util/public/AutoResetFunction.hpp"

namespace services
{
    class SpiMultipleAccessMaster                                                                               //TICS !OOP#013
        : public hal::SpiMaster
        , public infra::ClaimableResource
    {
    public:
        explicit SpiMultipleAccessMaster(hal::SpiMaster& master);

        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart) override;
        virtual uint32_t Speed() const override;
        virtual void ConfigSpeed(uint32_t speedInkHz) override;
        virtual uint8_t Mode() const override;
        virtual void ConfigMode(uint8_t spiMode) override;

    private:
        hal::SpiMaster& master;
    };

    class SpiMultipleAccess
        : public hal::SpiMaster
    {
    public:
        explicit SpiMultipleAccess(SpiMultipleAccessMaster& master);

        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart = infra::emptyFunction) override;
        virtual uint32_t Speed() const override;
        virtual void ConfigSpeed(uint32_t speedInkHz) override;
        virtual uint8_t Mode() const override;
        virtual void ConfigMode(uint8_t spiMode) override;

    private:
        void SendAndReceiveOnClaimed(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction);

    private:
        SpiMultipleAccessMaster& master;
        infra::ClaimableResource::Claimer::WithSize<36> claimer;
        infra::Function<void()> onDone;
        infra::Function<void()> actionOnStart;

        uint32_t speedInkHz;
        uint8_t spiMode;
    };
}

#endif
