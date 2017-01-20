#include "services/util/public/SpiMultipleAccess.hpp"

namespace services
{
    SpiMultipleAccessMaster::SpiMultipleAccessMaster(hal::SpiMaster& master)
        : master(master)
    {}

    void SpiMultipleAccessMaster::SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart)
    {
        master.SendAndReceive(sendData, receiveData, nextAction, actionOnCompletion, actionOnStart);
    }

    void SpiMultipleAccessMaster::SetCommunicationConfigurator(hal::CommunicationConfigurator& configurator)
    {
        if (communicationConfigurator != &configurator)
        {
            master.SetCommunicationConfigurator(configurator);
            communicationConfigurator = &configurator;
        }
    }

    void SpiMultipleAccessMaster::ResetCommunicationConfigurator()
    {
        if (communicationConfigurator)
        {
            master.ResetCommunicationConfigurator();
            communicationConfigurator = nullptr;
        }
    }

    SpiMultipleAccess::SpiMultipleAccess(SpiMultipleAccessMaster& master)
        : master(master)
        , claimer(master)
    {}

    void SpiMultipleAccess::SendAndReceive(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction, const infra::Function<void()>& actionOnCompletion, const infra::Function<void()>& actionOnStart)
    {
        this->actionOnStart = actionOnStart;
        this->onDone = actionOnCompletion;
        if (!claimer.IsClaimed())
            claimer.Claim([this, sendData, receiveData, nextAction]()
            {
                SendAndReceiveOnClaimed(sendData, receiveData, nextAction);
            });
        else
            SendAndReceiveOnClaimed(sendData, receiveData, nextAction);
    }

    void SpiMultipleAccess::SetCommunicationConfigurator(hal::CommunicationConfigurator& configurator)
    {
        communicationConfigurator = &configurator;
    }

    void SpiMultipleAccess::ResetCommunicationConfigurator()
    {
        communicationConfigurator = nullptr;
    }

    void SpiMultipleAccess::SendAndReceiveOnClaimed(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction)
    {
        if (communicationConfigurator)
            master.SetCommunicationConfigurator(*communicationConfigurator);
        else
            master.ResetCommunicationConfigurator();

        master.SendAndReceive(sendData, receiveData, nextAction, [this, nextAction]()
        {
            if (nextAction == hal::SpiAction::stop)
                claimer.Release();
            this->onDone();
        }, [this]()
        {
            this->actionOnStart();
        });
    }
}
