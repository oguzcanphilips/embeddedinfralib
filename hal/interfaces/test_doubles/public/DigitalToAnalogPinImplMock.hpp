#ifndef HAL_ANALOG_TO_DIGITAL_PIN_IMPL_MOCK_HPP
#define HAL_ANALOG_TO_DIGITAL_PIN_IMPL_MOCK_HPP

#include "gmock/gmock.h"
#include "hal/interfaces/public/DigitalToAnalogPin.hpp"

namespace hal
{
    class DigitalToAnalogPinImplMock
        : DigitalToAnalogPinImplBase
    {
    public:
        MOCK_METHOD1(Set, void(uitn16_t value));
    };
}

#endif
