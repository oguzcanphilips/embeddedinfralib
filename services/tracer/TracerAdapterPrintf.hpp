#ifndef SERVICES_TRACER_ADAPTER_PRINTF_HPP
#define SERVICES_TRACER_ADAPTER_PRINTF_HPP

#include <cstdarg>
#include "services/tracer/Tracer.hpp"

namespace services
{
    class TracerAdapterPrintf
    {
    public:
        explicit TracerAdapterPrintf(services::Tracer& tracer);

        void Print(const char* format, va_list args);

    private:
        void HandleFormat(const char*& format, va_list& args);
        void ReadLength(const char*& format, int& lengthSpecifier);
        void SkipSize(const char*& format);
        void ParseFormat(char format, int lengthSpecifier, va_list& args);

    private:
        services::Tracer& tracer;
    };
}

#endif
