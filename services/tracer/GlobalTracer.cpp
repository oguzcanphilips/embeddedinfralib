#include "infra/stream/IoOutputStream.hpp"
#include "services/tracer/Tracer.hpp"

namespace services
{
    namespace
    {
        Tracer* globalTracerInstance = nullptr;

#ifdef _MSC_VER
        infra::IoOutputStream ioOutputStream;
        Tracer tracerDummy(ioOutputStream);
#endif
    }

    void SetGlobalTracerInstance(Tracer& tracer)
    {
        assert(globalTracerInstance == nullptr);
        globalTracerInstance = &tracer;

    }

    Tracer& GlobalTracer()
    {
#ifdef _MSC_VER
        if (globalTracerInstance == nullptr)
            return tracerDummy;
#endif

        assert(globalTracerInstance != nullptr);
        return *globalTracerInstance;
    }
}
