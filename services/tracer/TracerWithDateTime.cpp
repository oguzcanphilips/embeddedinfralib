#include "infra/stream/StreamManipulators.hpp"
#include "services/tracer/TracerWithDateTime.hpp"

namespace services
{
    TracerWithDateTime::TracerWithDateTime(infra::TextOutputStream& stream, const infra::TimerService& timerService)
        : Tracer(stream)
        , timerService(timerService)
    {}

    void TracerWithDateTime::InsertHeader()
    {
        infra::TimePoint nowTimePoint = timerService.Now();
        time_t now = std::chrono::system_clock::to_time_t(nowTimePoint);
        tm utcTime = *gmtime(&now);

        Continue() << (utcTime.tm_year + 1900) << '-' << infra::Width(2, '0') << (utcTime.tm_mon + 1) << '-' << utcTime.tm_mday << ' '
            << utcTime.tm_hour << ':' << utcTime.tm_min << ':' << utcTime.tm_sec  << '.'
            << infra::Width(6, '0') << std::chrono::duration_cast<std::chrono::microseconds>(nowTimePoint.time_since_epoch()).count() % 1000000 << ' ';
    }
}
