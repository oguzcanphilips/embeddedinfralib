#include "gtest/gtest.h"
#include "hal/interfaces/test_doubles/public/GpioStub.hpp"
#include "infra/timer/test_helper/public/ClockFixture.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"
#include "services/util/public/GpioPinInverted.hpp"

class InverseLogicPinTest
    : public testing::Test
    , public infra::ClockFixture
{
public:
    InverseLogicPinTest()
        : gpioPin(basePin)
    {}

    infra::MockCallback<void()> callback;
    hal::GpioPinSpy basePin;
    services::GpioPinInverted gpioPin;
};

TEST_F(InverseLogicPinTest, GpioPinGetValue)
{
    hal::InputPin pin(gpioPin);

    basePin.SetStubState(true);
    EXPECT_FALSE(pin.Get());
    basePin.SetStubState(false);
    EXPECT_TRUE(pin.Get());
}

TEST_F(InverseLogicPinTest, GpioPinTriggerOnChange)
{
    hal::InputPin pin(gpioPin);

    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback());

    pin.EnableInterrupt([&callback]() { callback.callback(); }, hal::InterruptTrigger::fallingEdge);
    basePin.SetStubState(true);
}

TEST_F(InverseLogicPinTest, GpioPinSetValue)
{
    hal::OutputPin pin(gpioPin);

    pin.Set(true);
    EXPECT_FALSE(basePin.GetStubState());
    pin.Set(false);
    EXPECT_TRUE(basePin.GetStubState());
}

TEST_F(InverseLogicPinTest, GpioPinGetOutputValue)
{
    hal::OutputPin pin(gpioPin);

    pin.Set(true);
    EXPECT_TRUE(pin.GetOutputLatch());
    pin.Set(false);
    EXPECT_FALSE(pin.GetOutputLatch());
}