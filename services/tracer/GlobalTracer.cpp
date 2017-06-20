#include "services/tracer/Tracer.hpp"

namespace
{
    static services::Tracer* globalTracerInstance = nullptr;
}

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
