#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/event/public/EventDispatcher.hpp"
#include "infra/event/test_helper/public/EventDispatcherWithWeakPtrFixture.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"
#include "infra/util/public/SharedObjectAllocatorFixedSize.hpp"

class EventDispatcherWithWeakPtrTest
    : public testing::Test
    , public infra::EventDispatcherWithWeakPtrFixture
{};

TEST_F(EventDispatcherWithWeakPtrTest, TestSchedule)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback());

    infra::EventDispatcher::Instance().Schedule([&callback, this]() { callback.callback(); });
    ExecuteAllActions();
}

TEST_F(EventDispatcherWithWeakPtrTest, TestScheduleTwice)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback()).Times(2);

    infra::EventDispatcher::Instance().Schedule([&callback, this]() { callback.callback(); });
    infra::EventDispatcher::Instance().Schedule([&callback, this]() { callback.callback(); });
    ExecuteAllActions();
}

TEST_F(EventDispatcherWithWeakPtrTest, TestScheduleSharedPtr)
{
    infra::MockCallback<void()> callback;
    EXPECT_CALL(callback, callback());

    infra::SharedObjectAllocatorFixedSize<int, void()>::WithStorage<2> allocator;
    infra::SharedPtr<int> object = allocator.Allocate();
    infra::EventDispatcherWithWeakPtr::Instance().Schedule([&callback, this](const infra::SharedPtr<int>& object) { callback.callback(); }, object);
    ExecuteAllActions();
}

TEST_F(EventDispatcherWithWeakPtrTest, TestScheduleDestructedSharedPtr)
{
    testing::StrictMock<infra::MockCallback<void()>> callback;

    infra::SharedObjectAllocatorFixedSize<int, void()>::WithStorage<2> allocator;
    infra::SharedPtr<int> object = allocator.Allocate();
    infra::EventDispatcherWithWeakPtr::Instance().Schedule([&callback, this](const infra::SharedPtr<int>& object) { callback.callback(); }, object);
    object = nullptr;
    ExecuteAllActions();
}
