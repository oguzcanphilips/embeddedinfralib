#include "DataTransport.hpp"

namespace services
{

    DataTransportObserver::DataTransportObserver(DataTransport& dataTransport)
        : SingleObserver<DataTransportObserver, DataTransport>(dataTransport)
    {
    }

    DataTransportObserver::DataTransportObserver()
        : SingleObserver<DataTransportObserver, DataTransport>()
    {
    }

    BulkDataObserver::BulkDataObserver(BulkData& subject)
        : DataTransportObserver(subject)
    {}
}
