#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/event/public/EventDispatcher.hpp"
#include "infra/event/test_helper/public/EventDispatcherFixture.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"

class EventDispatcherFixture
    : public testing::Test
    , public infra::EventDispatcherFixture
{};

TEST_F(EventDispatcherFixture, TestConstruction)
{}

TEST_F(EventDispatcherFixture, TestSchedule)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback());

    infra::EventDispatcher::Instance().Schedule([&callback, this]() { callback.callback(); });
    ExecuteAllActions();
}

TEST_F(EventDispatcherFixture, TestScheduleTwice)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).Times(2);

    infra::EventDispatcher::Instance().Schedule([&callback, this]() { callback.callback(); });
    infra::EventDispatcher::Instance().Schedule([&callback, this]() { callback.callback(); });
    ExecuteAllActions();
}

TEST_F(EventDispatcherFixture, TestScheduleUnique)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback());

    infra::Function<void()> f([&callback, this]() { callback.callback(); });
    infra::EventDispatcher::Instance().ScheduleUnique(f);
    infra::EventDispatcher::Instance().ScheduleUnique(f);
    ExecuteAllActions();
}

bool helper1turn = true;

void helper1()
{
    really_assert(helper1turn);
    helper1turn = false;
}

void helper2()
{
    really_assert(!helper1turn);
    helper1turn = true;
}

TEST_F(EventDispatcherFixture, TestPerformance)
{
    for (int j = 0; j != 10; ++j)
    {
        for (int i = 0; i != 20; ++i)
        {
            infra::EventDispatcher::Instance().Schedule([]() { helper1(); });
            infra::EventDispatcher::Instance().Schedule([]() { helper2(); });
        }

        ExecuteAllActions();
    }
}
