#include "gtest/gtest.h"
#include "infra/util/public/AllocatorHeap.hpp"

class HeapAllocatorTest
    : public testing::Test
{
public:
    struct TestObject
    {
        TestObject()
        {
            ++numObjects;
        }

        ~TestObject()
        {
            --numObjects;
        }

        static int numObjects;
    };

    infra::AllocatorHeap<TestObject> allocator;
};

int HeapAllocatorTest::TestObject::numObjects = 0;

TEST_F(HeapAllocatorTest, Allocate)
{
    infra::UniquePtr<TestObject> object = allocator.Allocate();

    EXPECT_EQ(1, TestObject::numObjects);
}

TEST_F(HeapAllocatorTest, DeallocatedAfterDestructionOfPointer)
{
    {
        infra::UniquePtr<TestObject> object = allocator.Allocate();
    }

    EXPECT_EQ(0, TestObject::numObjects);
}
