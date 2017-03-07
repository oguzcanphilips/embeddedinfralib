#ifndef HAL_DIGITAL_TO_ANALOG_PIN_HPP
#define HAL_DIGITAL_TO_ANALOG_PIN_HPP

#include <stdint.h>

namespace hal
{
    class DigitalToAnalogPinImplBase
    {
    protected:
        ~DigitalToAnalogPinImplBase() = default;

    public:
        virtual void Set(uint16_t value) = 0;
    };
}

#endif
