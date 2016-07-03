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

    uint32_t SpiMultipleAccessMaster::Speed() const
    {
        return master.Speed();
    }

    void SpiMultipleAccessMaster::ConfigSpeed(uint32_t speedInkHz)
    {
        master.ConfigSpeed(speedInkHz);
    }

    uint8_t SpiMultipleAccessMaster::Mode() const
    {
        return master.Mode();
    }

    void SpiMultipleAccessMaster::ConfigMode(uint8_t spiMode)
    {
        master.ConfigMode(spiMode);
    }

    SpiMultipleAccess::SpiMultipleAccess(SpiMultipleAccessMaster& master)
        : master(master)
        , claimer(master)
        , speedInkHz(master.Speed())
        , spiMode(master.Mode())
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

    uint32_t SpiMultipleAccess::Speed() const
    {
        return speedInkHz;
    }

    void SpiMultipleAccess::ConfigSpeed(uint32_t speedInkHz)
    {
        this->speedInkHz = speedInkHz;
    }

    uint8_t SpiMultipleAccess::Mode() const
    {
        return spiMode;
    }

    void SpiMultipleAccess::ConfigMode(uint8_t spiMode)
    {
        this->spiMode = spiMode;
    }

    void SpiMultipleAccess::SendAndReceiveOnClaimed(infra::ConstByteRange sendData, infra::ByteRange receiveData, hal::SpiAction nextAction)
    {
        master.SendAndReceive(sendData, receiveData, nextAction, [this, nextAction]()
        {
            if (nextAction == hal::SpiAction::stop)
                claimer.Release();
            this->onDone();
        }, [this]()
        {
            this->actionOnStart();
            master.ConfigSpeed(speedInkHz);
            master.ConfigMode(spiMode);
        });
    }
}
