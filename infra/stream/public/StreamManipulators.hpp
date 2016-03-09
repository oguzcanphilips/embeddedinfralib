#ifndef INFRA_STREAM_MANIPULATORS_HPP
#define INFRA_STREAM_MANIPULATORS_HPP

#include <cstdlib>

namespace infra
{
    struct Text {};
    struct Hex {};
    struct Data {};

    const Text text;
    const Hex hex;
    const Data data;

    struct Width
    {
        explicit Width(std::size_t width);

        std::size_t width;
    };

    struct ForwardStream
    {
        explicit ForwardStream(std::size_t amount);

        std::size_t amount;
    };

    ////    Implementation    ////

    inline Width::Width(std::size_t width)
        : width(width)
    {}

    inline ForwardStream::ForwardStream(std::size_t amount)
        : amount(amount)
    {}
}

#endif
