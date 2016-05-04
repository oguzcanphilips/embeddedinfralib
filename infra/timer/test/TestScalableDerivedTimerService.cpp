#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/event/public/EventDispatcher.hpp"
#include "infra/timer/public/ScalableDerivedTimerService.hpp"
#include "infra/timer/public/Timer.hpp"
#include "infra/timer/public/TimerServiceManager.hpp"
#include "infra/timer/test_helper/public/ClockFixture.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"

class ScalableDerivedTimerServiceTest
    : public testing::Test
    , public infra::ClockFixture
{
public:
    ScalableDerivedTimerServiceTest()
        : scaleId(reinterpret_cast<uint32_t>(&scaleId))
        , scalableTimerService(scaleId, systemTimerService)
    {}

    uint32_t scaleId;
    infra::ScalableDerivedTimerService scalableTimerService;
};

TEST_F(ScalableDerivedTimerServiceTest, ShiftScalableTimerService)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(2)));

    infra::TimerSingleShot timer(std::chrono::seconds(3), [&callback]() { callback.callback(); }, scaleId);
    scalableTimerService.Shift(std::chrono::seconds(1));

    ForwardTime(std::chrono::seconds(5));
}

TEST_F(ScalableDerivedTimerServiceTest, ScaleScalableTimerService)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(2)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(3)));

    scalableTimerService.Scale(2);
    infra::TimerRepeating timer(std::chrono::seconds(2), [&callback]() { callback.callback(); }, scaleId);

    ForwardTime(std::chrono::seconds(3));
}

TEST_F(ScalableDerivedTimerServiceTest, ScaleAndShiftScalableTimerService)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));

    scalableTimerService.Scale(2);
    infra::TimerSingleShot timer(std::chrono::seconds(4), [&callback]() { callback.callback(); }, scaleId);
    scalableTimerService.Shift(std::chrono::seconds(1));

    ForwardTime(std::chrono::seconds(3));
}

TEST_F(ScalableDerivedTimerServiceTest, ScaleAndShiftScalableTimerServiceAfterSomeTimePassed)
{
    ForwardTime(std::chrono::seconds(10));

    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));

    scalableTimerService.Scale(2);
    infra::TimerSingleShot timer(std::chrono::seconds(4), [&callback]() { callback.callback(); }, scaleId);
    scalableTimerService.Shift(std::chrono::seconds(1));

    ForwardTime(std::chrono::seconds(3));
}

TEST_F(ScalableDerivedTimerServiceTest, ScaleAndScaleBackAfterSomeTimePassed)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(1)));
    EXPECT_CALL(callback, callback()).With(After(std::chrono::seconds(3)));

    scalableTimerService.Scale(2);
    infra::TimerRepeating timer(std::chrono::seconds(2), [&callback]() { callback.callback(); }, scaleId);
    ForwardTime(std::chrono::seconds(1));
    scalableTimerService.Scale(1);

    ForwardTime(std::chrono::seconds(2));
}