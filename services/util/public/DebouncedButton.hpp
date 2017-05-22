#ifndef SERVICES_DEBOUNCED_BUTTON_HPP
#define SERVICES_DEBOUNCED_BUTTON_HPP

#include "hal/interfaces/public/Gpio.hpp"
#include "infra/timer/public/Timer.hpp"

namespace services
{
    class DebouncedButton
    {
    public:
        DebouncedButton(hal::GpioPin& buttonPin, infra::Function<void()> callback, infra::Duration debounceDuration = std::chrono::milliseconds(10));

    private:
        void ButtonChanged();
        void ButtonPressed();

    private:
        hal::InputPin buttonPin;
        infra::Duration debounceDuration;
        bool previousButtonState;
        infra::TimerSingleShot debounceEnd;
        infra::Function<void()> callback;
    };
}

#endif
