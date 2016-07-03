#include "infra/stream/public/StreamManipulators.hpp"

namespace infra
{
    Width::Width(std::size_t width, char padding)
        : width(width)
        , padding(padding)
    {}

    ForwardStream::ForwardStream(std::size_t amount)
        : amount(amount)
    {}
}
