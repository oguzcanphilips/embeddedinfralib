#ifndef INFRA_STREAM_MANIPULATORS_HPP
#define INFRA_STREAM_MANIPULATORS_HPP

#include <cstdlib>

namespace infra
{
    extern const struct SoftFail {} softFail;
    extern const struct NoFail {} noFail;

    const struct Text {} text;
    const struct Hex {} hex;
    const struct Data {} data;
    const struct Endl {} endl;

    struct Width
    {
        explicit Width(std::size_t width, char padding = ' ');

        std::size_t width;
        char padding;
    };

    struct ForwardStream
    {
        explicit ForwardStream(std::size_t amount);

        std::size_t amount;
    };
}

#endif
