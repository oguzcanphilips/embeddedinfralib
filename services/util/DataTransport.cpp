#include "DataTransport.hpp"

namespace services
{
    DataTransportObserver::DataTransportObserver()
    {}

    BulkDataObserver::BulkDataObserver(BulkData& subject)
        : infra::SingleObserver<BulkDataObserver, BulkData>(subject)
    {}
}
