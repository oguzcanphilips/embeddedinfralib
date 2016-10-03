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

TEST_F(FixedSpaceAllocatorTest, ConstructedNotEmpty)
{
    EXPECT_FALSE(allocator.Empty());
}

TEST_F(FixedSpaceAllocatorTest, Allocate)
{
    infra::UniquePtr<TestObject> object = allocator.Allocate();

    EXPECT_TRUE(allocator.Empty());
}

TEST_F(FixedSpaceAllocatorTest, DeallocatedAfterDestructionOfPointer)
{
    {
        infra::UniquePtr<TestObject> object = allocator.Allocate();
    }

    EXPECT_FALSE(allocator.Empty());
}
