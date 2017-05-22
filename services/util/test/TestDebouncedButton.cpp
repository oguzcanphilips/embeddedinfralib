#include "gtest/gtest.h"
#include "hal/interfaces/test_doubles/public/GpioStub.hpp"
#include "infra/timer/test_helper/public/ClockFixture.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"
#include "services/util/public/DebouncedButton.hpp"

class DebouncedButtonFixtureBase
    : public testing::Test
    , public infra::ClockFixture
{
public:
    infra::Optional<services::DebouncedButton> debouncedButton;
    hal::GpioPinStub button;
    infra::MockCallback<void()> callback;
};

class DebouncedButtonFixture
    : public DebouncedButtonFixtureBase
{
public:
    DebouncedButtonFixture()
    {
        debouncedButton.Emplace(button, [this]() { callback.callback(); });
    }
};

TEST_F(DebouncedButtonFixture, TriggeredWhenPushed)
{
    EXPECT_CALL(callback, callback());

    button.SetStubState(true);
}

TEST_F(DebouncedButtonFixture, NotTriggeredWhenPushedInTheDebouncePeriod)
{
    EXPECT_CALL(callback, callback()).With(After(std::chrono::milliseconds(0)));

    button.SetStubState(true);
    button.SetStubState(false);
    ForwardTime(std::chrono::milliseconds(5));
    button.SetStubState(true);
}

TEST_F(DebouncedButtonFixture, TriggeredAgainWhenPushedOutsideTheDebouncePeriod)
{
    EXPECT_CALL(callback, callback()).With(After(std::chrono::milliseconds(0)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::milliseconds(10)));

    button.SetStubState(true);
    button.SetStubState(false);
    ForwardTime(std::chrono::milliseconds(10));
    button.SetStubState(true);
}
