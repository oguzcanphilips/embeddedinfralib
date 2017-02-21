#ifndef SERVICES_TRACER_HPP
#define SERVICES_TRACER_HPP

#include "infra/stream/public/OutputStream.hpp"

namespace services
{
    class Tracer
    {
    public:
        explicit Tracer(infra::TextOutputStream& stream);

        infra::TextOutputStream Trace();
        infra::TextOutputStream Continue();

    protected:
        virtual void InsertHeader();

    private:
        infra::TextOutputStream& stream;
    };
}

#endif
