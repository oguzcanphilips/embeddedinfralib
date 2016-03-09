#ifndef INFRA_STREAM_MANIPULATORS_HPP
#define INFRA_STREAM_MANIPULATORS_HPP

#include <cstdlib>

namespace infra
{
    struct ForwardStream
    {
        explicit ForwardStream(std::size_t amount);

        std::size_t amount;
    };

    ////    Implementation    ////

    inline ForwardStream::ForwardStream(std::size_t amount)
        : amount(amount)
    {}
}

#endif
