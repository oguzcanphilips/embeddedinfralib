#include "hal/interfaces/test_doubles/public/AnalogToDigitalPinImplMock.hpp"

namespace hal
{
    void AnalogToDigitalPinImplMock::Measure(const infra::Function<void(uint16_t value)>& onDone)
    {
        MeasureMock();
        this->measureDone = onDone;
    }
}
