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
        virtual void Measure(const infra::Function<void(uint16_t value)>& onDone) override;

        MOCK_METHOD0(MeasureMock, void());

        infra::Function<void(uint16_t value)> measureDone;
    };
}

#endif
