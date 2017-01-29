#ifndef HAL_ANALOG_TO_DIGITAL_PIN_HPP
#define HAL_ANALOG_TO_DIGITAL_PIN_HPP

#include "infra/util/public/AutoResetFunction.hpp"
#include "infra/util/public/Unit.hpp"

namespace hal
{
    template<class Unit, class Storage>
    class AnalogToDigitalPin
    {
    protected:
        AnalogToDigitalPin() = default;
        AnalogToDigitalPin(const AnalogToDigitalPin&) = delete;
        AnalogToDigitalPin& operator=(const AnalogToDigitalPin&) = delete;
        ~AnalogToDigitalPin() = default;

    public:
        virtual void Measure(const infra::Function<void(infra::Quantity<Unit, Storage> value)>& onDone) = 0;
    };

    class AnalogToDigitalPinImplBase
    {
    protected:
        ~AnalogToDigitalPinImplBase() = default;

    public:
        virtual void Measure(const infra::Function<void(uint16_t value)>& onDone) = 0;
    };

    template<class Conversion, class Unit, class Storage, class Impl>
    class AnalogToDigitalPinConverter
        : public AnalogToDigitalPin<Unit, Storage>
        , public Impl
    {
    public:
        template<class... Args>
            AnalogToDigitalPinConverter(Args&&... args);

        void Measure(const infra::Function<void(infra::Quantity<Unit, Storage> value)>& onDone);

    private:
        infra::AutoResetFunction<void(infra::Quantity<Unit, Storage> value)> onDone;
    };

    ////    Implementation    ////

    template<class Conversion, class Unit, class Storage, class Impl>
    template<class... Args>
    AnalogToDigitalPinConverter<Conversion, Unit, Storage, Impl>::AnalogToDigitalPinConverter(Args&&... args)
        : Impl(std::forward<Args>(args)...)
    {}

    template<class Conversion, class Unit, class Storage, class Impl>
    void AnalogToDigitalPinConverter<Conversion, Unit, Storage, Impl>::Measure(const infra::Function<void(infra::Quantity<Unit, Storage> value)>& onDone)
    {
        this->onDone = onDone;
        Impl::Measure([this](uint16_t value)
        {
            infra::Quantity<infra::Identity, Storage> convertedValue(value * infra::Quantity<Conversion, Storage>(1));
            this->onDone(convertedValue);
        });
    }
}

#endif
