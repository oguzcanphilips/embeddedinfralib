#ifndef INFRA_STREAM_MANIPULATORS_HPP
#define INFRA_STREAM_MANIPULATORS_HPP

#include <cstdlib>

namespace infra
{
    const struct SoftFail {} softFail;

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

    ////    Implementation    ////

    inline Width::Width(std::size_t width, char padding)
        : width(width)
        , padding(padding)
    {}

    inline ForwardStream::ForwardStream(std::size_t amount)
        : amount(amount)
    {}
}

#endif
