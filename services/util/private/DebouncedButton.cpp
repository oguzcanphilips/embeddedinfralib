#include "infra/timer/public/TimerServiceManager.hpp"
#include "services/util/public/DebouncedButton.hpp"

namespace services
{
    DebouncedButton::DebouncedButton(hal::GpioPin& buttonPin, infra::Function<void()> callback, infra::Duration debounceDuration)
        : buttonPin(buttonPin)
        , debounceDuration(debounceDuration)
        , previousButtonState(false)
        , callback(callback)
    {
        this->buttonPin.EnableInterrupt([this]() { ButtonChanged(); }, hal::InterruptTrigger::bothEdges);
    }

    void DebouncedButton::ButtonChanged()
    {
        bool buttonState = buttonPin.Get();

        if (previousButtonState != buttonState)
        {
            if (buttonState)
                ButtonPressed();
        }

        previousButtonState = buttonState;
    }

    void DebouncedButton::ButtonPressed()
    {
        if (!debounceEnd.Armed())
        {
            debounceEnd.Start(debounceDuration, infra::emptyFunction);
            callback();
        }
    }
}
