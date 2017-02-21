#ifndef HAL_ANALOG_TO_DIGITAL_PIN_STUB_HPP
#define HAL_ANALOG_TO_DIGITAL_PIN_STUB_HPP

#include "hal/interfaces/public/AnalogToDigitalPin.hpp"

namespace hal
{
    class AnalogToDigitalPinImplStub
        : public AnalogToDigitalPinImplBase
    {
    public:
        virtual void Measure(const infra::Function<void(uint16_t value)>& onDone) override;
    };
}

#endif
