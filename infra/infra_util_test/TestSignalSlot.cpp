#include "gtest/gtest.h"
#include "infra_util/SignalSlot.hpp"

class SignalSlotTest
    : public testing::Test
{};

TEST_F(SignalSlotTest, TestConstruction)
{
    struct Bla
    {
        void f(int x)
        {}

        void invokeSignal(infra::Signal<Bla, void(int)>& signal)
        {
            signal(5);
        }
    };

    Bla b;

    infra::Signal<Bla, void(int)> signal;
    infra::Slot<void(int)> slot([&b](int x) { b.f(x); } );

    signal += slot;

    b.invokeSignal(signal);
}
