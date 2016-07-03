#include "gtest/gtest.h"
#include "hal/interfaces/test_doubles/public/GpioStub.hpp"
#include "infra/timer/test_helper/public/ClockFixture.hpp"
#include "services/util/public/DebugLed.hpp"

class DebugLedFixtureBase
    : public testing::Test
    , public infra::ClockFixture
{
public:
    infra::Optional<services::DebugLed> debugLed;
    hal::GpioPinSpy led;
};

class DebugLedFixture
    : public DebugLedFixtureBase
{
public:
    DebugLedFixture()
    {
        debugLed.Emplace(led);
    }
};

TEST_F(DebugLedFixture, DebugLedContinuouslyToggles)
{
    ForwardTime(std::chrono::seconds(2));
    EXPECT_EQ((std::vector<hal::PinChange>{
        { std::chrono::milliseconds(800), true },
        { std::chrono::milliseconds(1000), false },
        { std::chrono::milliseconds(1800), true },
        { std::chrono::milliseconds(2000), false }
    }), led.PinChanges());
}

TEST_F(DebugLedFixtureBase, NonStandardDurations)
{
    debugLed.Emplace(led, std::chrono::milliseconds(20), std::chrono::milliseconds(80));

    ForwardTime(std::chrono::milliseconds(200));
    EXPECT_EQ((std::vector<hal::PinChange>{
        { std::chrono::milliseconds(80), true },
        { std::chrono::milliseconds(100), false },
        { std::chrono::milliseconds(180), true },
        { std::chrono::milliseconds(200), false }
    }), led.PinChanges());
}
