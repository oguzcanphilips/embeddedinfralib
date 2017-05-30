#include "gtest/gtest.h"
#include "infra/timer/TimerExponentialBackoff.hpp"
#include "infra/timer/test_helper/ClockFixture.hpp"
#include "infra/util/test_helper/MockCallback.hpp"

class TimerExponentialBackoffTest
    : public testing::Test
    , public infra::ClockFixture
{};

TEST_F(TimerExponentialBackoffTest, TimerWillNotTriggerWhenConditionIsFalse)
{
    infra::MockCallback<void()> action;

    infra::TimerExponentialBackoff timer;
    timer.Start(std::chrono::seconds(1), std::chrono::hours(1), [] { return false; }, [&action] { action.callback(); });

    ForwardTime(std::chrono::hours(1));
};

TEST_F(TimerExponentialBackoffTest, TimerWillTriggerAfterDurationExpiredAndConditionIsTrue)
{
    infra::MockCallback<void()> action;
    EXPECT_CALL(action, callback()).With(After(std::chrono::seconds(1)));
    EXPECT_CALL(action, callback()).With(After(std::chrono::seconds(2)));

    infra::TimerExponentialBackoff timer;
    timer.Start(std::chrono::seconds(1), std::chrono::hours(1), [] { return true; }, [&action] { action.callback(); });

    ForwardTime(std::chrono::seconds(2));
};

TEST_F(TimerExponentialBackoffTest, TimerWillIncreaseDurationAfterConditionIsFalse)
{
    infra::MockCallback<void()> action;
    infra::MockCallback<bool()> condition;

    testing::InSequence s;
    EXPECT_CALL(condition, callback()).Times(3).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(condition, callback()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(action, callback()).With(After(std::chrono::seconds(8)));

    infra::TimerExponentialBackoff timer;
    timer.Start(std::chrono::seconds(1), std::chrono::hours(1), [&condition] { return condition.callback(); }, [&action] { action.callback(); });

    ForwardTime(std::chrono::seconds(8));
};

TEST_F(TimerExponentialBackoffTest, TimerWillIncreaseDurationAfterConditionIsFalseUpToMaximumBackoff)
{
    infra::MockCallback<void()> action;
    infra::MockCallback<bool()> condition;

    testing::InSequence s;
    EXPECT_CALL(condition, callback()).Times(5).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(condition, callback()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(action, callback()).With(After(std::chrono::seconds(20)));

    infra::TimerExponentialBackoff timer;
    timer.Start(std::chrono::seconds(1), std::chrono::seconds(10), [&condition] { return condition.callback(); }, [&action] { action.callback(); });

    ForwardTime(std::chrono::seconds(20));
};
