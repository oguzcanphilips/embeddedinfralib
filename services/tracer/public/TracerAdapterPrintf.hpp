#ifndef SERVICES_TRACER_ADAPTER_PRINTF_HPP
#define SERVICES_TRACER_ADAPTER_PRINTF_HPP

#include <cstdarg>
#include "services/tracer/public/Tracer.hpp"

namespace services
{
    class TracerAdapterPrintf
    {
    public:
        explicit TracerAdapterPrintf(services::Tracer& tracer);

        int Print(const char* format, va_list args);

    private:
        services::Tracer& tracer;
    };
}

#endif
