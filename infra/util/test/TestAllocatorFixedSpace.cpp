#include "gtest/gtest.h"
#include "infra/util/public/AllocatorFixedSpace.hpp"

class FixedSpaceAllocatorTest
    : public testing::Test
{
public:
    struct TestObject
    {};

    infra::AllocatorFixedSpace<TestObject, 1> allocator;
};

TEST_F(FixedSpaceAllocatorTest, allocator_is_constructed_not_empty)
{
    EXPECT_FALSE(allocator.Empty());
}

TEST_F(FixedSpaceAllocatorTest, allocate_one_object)
{
    infra::UniquePtr<TestObject> object = allocator.Allocate();

    EXPECT_TRUE(allocator.Empty());
}

TEST_F(FixedSpaceAllocatorTest, after_object_goes_out_of_scope_object_is_released)
{
    {
        infra::UniquePtr<TestObject> object = allocator.Allocate();
    }

    EXPECT_FALSE(allocator.Empty());
}

TEST_F(FixedSpaceAllocatorTest, when_allocating_more_than_capacity_nullptr_is_returned)
{
    infra::UniquePtr<TestObject> firstObject = allocator.Allocate();
    infra::UniquePtr<TestObject> secondObject = allocator.Allocate();

    EXPECT_EQ(nullptr, secondObject);
}
