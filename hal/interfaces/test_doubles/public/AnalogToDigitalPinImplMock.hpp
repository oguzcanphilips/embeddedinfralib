#ifndef HAL_ANALOG_TO_DIGITAL_PIN_IMPL_MOCK_HPP
#define HAL_ANALOG_TO_DIGITAL_PIN_IMPL_MOCK_HPP

#include "gmock/gmock.h"
#include "hal/interfaces/public/AnalogToDigitalPin.hpp"

namespace hal
{
    class AnalogToDigitalPinImplMock
        : public AnalogToDigitalPinImplBase
    {
    public:
        MOCK_METHOD1(Measure, void(const infra::Function<void(uint16_t value)>& onDone));
    };
}

#endif
