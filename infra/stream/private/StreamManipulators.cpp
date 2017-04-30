#include "infra/stream/public/StreamManipulators.hpp"

namespace infra
{
    const SoftFail softFail;
    const NoFail noFail;

    const Text text;
    const Hex hex;
    const Bin bin;
    const Data data;
    const Endl endl;

    Width::Width(std::size_t width, char padding)
        : width(width)
        , padding(padding)
    {}
}
