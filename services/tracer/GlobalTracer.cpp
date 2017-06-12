#include "services/tracer/Tracer.hpp"

namespace services
{
    Tracer* globalTracerInstance = nullptr;

    void SetGlobalTracerInstance(Tracer& tracer)
    {
        assert(globalTracerInstance == nullptr);
        globalTracerInstance = &tracer;

    }

    Tracer& GlobalTracer()
    {
        assert(globalTracerInstance != nullptr);
        return *globalTracerInstance;
    }
}
