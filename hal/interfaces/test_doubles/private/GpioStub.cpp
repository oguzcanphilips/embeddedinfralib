#include "hal/interfaces/test_doubles/public/GpioStub.hpp"
#include "infra/timer/public/TimerServiceManager.hpp"
#include "infra/util/public/CompareMembers.hpp"

namespace hal
{
    bool GpioPinStub::Get() const
    {
        return state;
    }

    void GpioPinStub::Set(bool value)
    {
        input = false;
        if (state != value)
        {
            state = value;

            if (triggerOnChange)
            if ((value && triggerOnChange->second != InterruptTrigger::fallingEdge)
                || (!value && triggerOnChange->second != InterruptTrigger::risingEdge))
                triggerOnChange->first();
        }
    }

    bool GpioPinStub::GetOutputLatch() const
    {
        return state;
    }

    void GpioPinStub::SetAsInput()
    {
        input = true;
    }

    bool GpioPinStub::IsInput() const
    {
        return input;
    }

    void GpioPinStub::Config(PinConfigType config)
    {
        input = true;
    }

    void GpioPinStub::Config(PinConfigType config, bool startOutputState)
    {
        input = false;
        SetStubState(startOutputState);
    }

    void GpioPinStub::ResetConfig()
    {}

    void GpioPinStub::EnableInterrupt(const infra::Function<void()>& actionOnInterrupt, InterruptTrigger trigger)
    {
        triggerOnChange = std::make_pair(actionOnInterrupt, trigger);
    }

    void GpioPinStub::DisableInterrupt()
    {
        triggerOnChange = infra::none;
    }

    void GpioPinStub::SetStubState(bool value)
    {
        input = false;
        if (state != value)
        {
            state = value;

            if (triggerOnChange)
                if ((value && triggerOnChange->second != InterruptTrigger::fallingEdge)
                    || (!value && triggerOnChange->second != InterruptTrigger::risingEdge))
                    triggerOnChange->first();
        }
    }

    bool GpioPinStub::GetStubState() const
    {
        return state;
    }

    PinChange::PinChange(infra::Duration duration, bool state)
        : duration(duration)
        , state(state)
    {}

    bool PinChange::operator==(const PinChange& other) const
    {
        return infra::Equals()
            (duration, other.duration)
            (state, other.state)
            ;
    }

    bool PinChange::operator!=(const PinChange& other) const
    {
        return !(*this == other);
    }

    GpioPinSpy::GpioPinSpy()
        : start(infra::TimerServiceManager::Instance().GetTimerService(infra::systemTimerServiceId).Now())
    {}

    void GpioPinSpy::Set(bool value)
    {
        if (GetOutputLatch() != value)
            pinChanges.push_back({ infra::TimerServiceManager::Instance().GetTimerService(infra::systemTimerServiceId).Now() - start, value });

        GpioPinStub::Set(value);
    }
    
    std::vector<PinChange> GpioPinSpy::PinChanges() const
    {
        return pinChanges;
    }
}
