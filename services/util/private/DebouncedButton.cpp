#include "infra/timer/public/TimerServiceManager.hpp"
#include "services/util/public/DebouncedButton.hpp"

namespace services
{
    DebouncedButton::DebouncedButton(hal::GpioPin& buttonPin, infra::Function<void()> onPressed, infra::Function<void()> onReleased, infra::Duration debounceDuration)
        : buttonPin(buttonPin)
        , debounceDuration(debounceDuration)
        , previousButtonState(false)
        , onPressed(onPressed)
        , onReleased(onReleased)
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
            else
                ButtonReleased();
        }

        previousButtonState = buttonState;
    }

    void DebouncedButton::ButtonPressed()
    {
        if (!debounceEnd.Armed())
        {
            debounceEnd.Start(debounceDuration, [this]() { DebounceEnd(); });
            announcedPressed = true;
            onPressed();
        }
    }

    void DebouncedButton::ButtonReleased()
    {
        if (!debounceEnd.Armed())
        {
            debounceEnd.Start(debounceDuration, [this]() { DebounceEnd(); });
            announcedPressed = false;
            onReleased();
        }
    }

    void DebouncedButton::DebounceEnd()
    {
        if (announcedPressed && !previousButtonState)
            onReleased();
        else if (!announcedPressed && previousButtonState)
            onPressed();
    }
}
