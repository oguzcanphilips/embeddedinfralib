#include "gtest/gtest.h"
#include "infra/timer/public/Timer.hpp"
#include "infra/timer/test_helper/public/ClockFixture.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"

class TimerTest
    : public testing::Test
    , public infra::ClockFixture
{};

TEST_F(TimerTest, SingleShotTimerTriggersOnceAfterDuration)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));

    infra::TimerSingleShot timer(std::chrono::seconds(1), [&callback]() { callback.callback(); });

    ForwardTime(std::chrono::seconds(5));
};

TEST_F(TimerTest, SingleShotTimerIsCancellable)
{
    infra::MockCallback<void(infra::TimePoint)> callback;

    infra::TimerSingleShot timer(std::chrono::seconds(1), [this, &callback]() { callback.callback(systemTimerService.Now()); });

    timer.Cancel();

    ForwardTime(std::chrono::seconds(5));
};

TEST_F(TimerTest, SingleShotTimerTakesResolutionIntoAccount)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(2)));

    systemTimerService.SetResolution(std::chrono::seconds(1));
    infra::TimerSingleShot timer(std::chrono::seconds(1), [&callback]() { callback.callback(); });

    ForwardTime(std::chrono::seconds(5));
};

TEST_F(TimerTest, TwoTimersTriggerInOrder)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(2)));

    infra::TimerSingleShot timer1(std::chrono::seconds(1), [&callback]() { callback.callback(); });
    infra::TimerSingleShot timer2(std::chrono::seconds(2), [&callback]() { callback.callback(); });

    ForwardTime(std::chrono::seconds(5));
};

TEST_F(TimerTest, RepeatingTimerTriggersRepeatedly)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(2)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(3)));

    infra::TimerRepeating timer(std::chrono::seconds(1), [&callback]() { callback.callback(); });

    ForwardTime(std::chrono::seconds(3));
};

TEST_F(TimerTest, RepeatingTimerTriggersImmediately)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(0)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));

    infra::TimerRepeating timer(std::chrono::seconds(1), [&callback]() { callback.callback(); }, infra::triggerImmediately);

    ForwardTime(std::chrono::seconds(1));
}

TEST_F(TimerTest, RepeatingTimerTriggersRepeatedlyAfterReset)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(2)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(3)));

    infra::TimerRepeating timer;
    timer.Start(std::chrono::seconds(1), [&callback]() { callback.callback(); });

    ForwardTime(std::chrono::seconds(3));
};

TEST_F(TimerTest, RepeatingTimerTriggersImmediatelyAfterReset)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(0)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));

    infra::TimerRepeating timer;
    timer.Start(std::chrono::seconds(1), [&callback]() { callback.callback(); }, infra::triggerImmediately);

    ForwardTime(std::chrono::seconds(1));
}

TEST_F(TimerTest, RepeatingTimerIsCancellable)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));

    infra::TimerRepeating timer(std::chrono::seconds(1), [&callback]() { callback.callback(); });

    ForwardTime(std::chrono::seconds(1));
    timer.Cancel();
    ForwardTime(std::chrono::seconds(1));
};

TEST_F(TimerTest, RepeatingTimerTakesResolutionIntoAccount)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(2)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(3)));
    systemTimerService.SetResolution(std::chrono::seconds(1));

    infra::TimerRepeating timer(std::chrono::seconds(1), [&callback]() { callback.callback(); });

    ForwardTime(std::chrono::seconds(3));
};

TEST_F(TimerTest, TestTimerNotArmedAfterConstruction)
{
    infra::TimerSingleShot timer;

    EXPECT_FALSE(timer.Armed());
}

TEST_F(TimerTest, TestTimerArmed)
{
    infra::TimerSingleShot timer(std::chrono::seconds(1), []() {});

    EXPECT_TRUE(timer.Armed());
}

TEST_F(TimerTest, TestTimerNotArmedAfterTrigger)
{
    infra::TimerSingleShot timer(std::chrono::seconds(1), []() {});

    ForwardTime(std::chrono::seconds(5));
    EXPECT_FALSE(timer.Armed());
}

TEST_F(TimerTest, TestTimerStaysArmedAfterRepeatedTrigger)
{
    infra::TimerRepeating timer(std::chrono::seconds(1), []() {});

    ForwardTime(std::chrono::seconds(5));
    EXPECT_TRUE(timer.Armed());
}

TEST_F(TimerTest, TestTimerCancel)
{
    testing::StrictMock<infra::MockCallback<void()>> callback;

    infra::TimerSingleShot timer(std::chrono::seconds(1), [&callback]() { callback.callback();  });
    timer.Cancel();

    ForwardTime(std::chrono::seconds(5));
}

TEST_F(TimerTest, TestTimerIsNotArmedAfterCancel)
{
    infra::TimerSingleShot timer(std::chrono::seconds(1), []() {});
    timer.Cancel();

    EXPECT_FALSE(timer.Armed());
}

TEST_F(TimerTest, set_timer_before_roll_over_to_trigger_after_roll_over)
{
    ForwardTime(std::chrono::milliseconds(std::numeric_limits<uint32_t>::max() - 10));

    testing::StrictMock<infra::MockCallback<void()>> callback;
    infra::TimerSingleShot timer(std::chrono::milliseconds(20), [&callback]() { callback.callback();  });
    EXPECT_EQ(infra::TimePoint() + std::chrono::milliseconds(static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) + 10), timer.NextTrigger());

    ForwardTime(std::chrono::milliseconds(15));
    testing::Mock::VerifyAndClearExpectations(&callback);

    EXPECT_CALL(callback, callback());
    ForwardTime(std::chrono::milliseconds(5));
}
