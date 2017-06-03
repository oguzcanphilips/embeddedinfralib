#ifndef INFRA_MOCK_HELPERS_HPP
#define INFRA_MOCK_HELPERS_HPP

#include "gmock/gmock.h"

namespace infra
{
    ACTION_TEMPLATE(SaveRef,
        HAS_1_TEMPLATE_PARAMS(int, k),
        AND_1_VALUE_PARAMS(pointer)) {
        *pointer = &::std::tr1::get<k>(args);
    }
}

#endif
