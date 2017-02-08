#ifndef SERVICES_TRACER_WITH_DATE_TIME_HPP
#define SERVICES_TRACER_WITH_DATE_TIME_HPP

#include "infra/timer/public/TimerService.hpp"
#include "services/tracer/public/Tracer.hpp"

namespace services
{
    class TracerWithDateTime
        : public Tracer
    {
    public:
        TracerWithDateTime(infra::TextOutputStream& stream, const infra::TimerService& timerService);

    protected:
        virtual void InsertHeader() override;

    private:
        const infra::TimerService& timerService;
    };
}

#endif
