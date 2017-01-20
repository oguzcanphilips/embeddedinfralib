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
        virtual void SetCommunicationConfigurator(hal::CommunicationConfigurator& configurator) override;
        virtual void ResetCommunicationConfigurator() override;

    private:
        hal::SpiMaster& master;
        hal::CommunicationConfigurator* communicationConfigurator = nullptr;
    };

    class SpiMultipleAccess
        : public hal::SpiMaster
    {
    public:
        explicit SpiMultipleAccess(SpiMultipleAccessMaster& master);

        virtual void SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart = infra::emptyFunction) override;
        virtual void SetCommunicationConfigurator(hal::CommunicationConfigurator& configurator) override;
        virtual void ResetCommunicationConfigurator() override;

    private:
        void SendAndReceiveOnClaimed(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction);

    private:
        SpiMultipleAccessMaster& master;
        infra::ClaimableResource::Claimer::WithSize<36> claimer;
        infra::Function<void()> onDone;
        infra::Function<void()> actionOnStart;
        hal::CommunicationConfigurator* communicationConfigurator = nullptr;
    };
}

#endif
