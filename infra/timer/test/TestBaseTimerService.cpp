#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/event/test_helper/public/EventDispatcherFixture.hpp"
#include "infra/timer/public/BaseTimerService.hpp"
#include "infra/timer/public/Timer.hpp"
#include "infra/timer/public/TimerServiceManager.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"

class BaseTimerServiceTest
    : public testing::Test
    , public infra::EventDispatcherFixture
{
public:
    BaseTimerServiceTest()
        : timerService(infra::systemTimerServiceId, std::chrono::seconds(1))
    {}

    infra::BaseTimerService timerService;
};

TEST_F(BaseTimerServiceTest, set_resolution)
{
    infra::VerifyingFunctionMock<void()> verify;
    infra::TimerSingleShot timer(std::chrono::seconds(1), verify);

    timerService.SetResolution(std::chrono::seconds(2));
    ASSERT_EQ(infra::TimePoint(), timerService.Now());
    timerService.SystemTickInterrupt();
    ExecuteAllActions();
    EXPECT_EQ(infra::TimePoint() + std::chrono::seconds(2), timerService.Now());
}
