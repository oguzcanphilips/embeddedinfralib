#ifndef INFRA_UTIL_MATCHERS_HPP
#define INFRA_UTIL_MATCHERS_HPP

#include "gmock/gmock.h"
#include "infra/util/BoundedString.hpp"

namespace testing
{
    template<class T>
    class Matcher<infra::BoundedStringBase<T>>
        : public internal::MatcherBase<infra::BoundedStringBase<T>>
    {
    public:
        Matcher() = default;

        explicit Matcher(const MatcherInterface<const infra::BoundedStringBase<T>&>* impl)
            : internal::MatcherBase<infra::BoundedStringBase<T>>(impl)
        {}

        explicit Matcher(const MatcherInterface<infra::BoundedStringBase<T>>* impl)
            : internal::MatcherBase<infra::BoundedStringBase<T>>(impl)
        {}

        Matcher(infra::BoundedStringBase<T> s) { *this = Eq(std::string(s.data(), s.size())); }
        Matcher(const char* s) { *this = Eq(std::string(s)); }
    };
}

#endif
