#include "ZeroCopyDataTransport.hpp"

namespace services
{

    ZeroCopyDataTransportObserver::ZeroCopyDataTransportObserver(ZeroCopyDataTransport& dataTransport)
        : SingleObserver<ZeroCopyDataTransportObserver, ZeroCopyDataTransport>(dataTransport)
    {
    }

    ZeroCopyDataTransportObserver::ZeroCopyDataTransportObserver()
        : SingleObserver<ZeroCopyDataTransportObserver, ZeroCopyDataTransport>()
    {
    }

}