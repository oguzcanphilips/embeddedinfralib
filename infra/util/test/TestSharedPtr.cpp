#include <gmock/gmock.h>
#include "infra/util/public/SharedPtr.hpp"
#include "infra/util/public/SharedObjectAllocatorFixedSize.hpp"

class ObjectConstructionMock
{
public:
    MOCK_METHOD0(Construct, void());
    MOCK_METHOD0(Destruct, void());
};

class MySharedObjectBase
{
protected:
    MySharedObjectBase() = default;
    MySharedObjectBase(const MySharedObjectBase& other) = delete;
    MySharedObjectBase& operator=(const MySharedObjectBase& other) = delete;
    ~MySharedObjectBase() = default;
};

class MySharedObject
    : public MySharedObjectBase
{
public:
    MySharedObject(ObjectConstructionMock& mock)
        : mock(mock)
    {
        mock.Construct();
    }

    ~MySharedObject()
    {
        mock.Destruct();
    }

    int Value() const
    {
        return 5;
    }

private:
    ObjectConstructionMock& mock;
};

class SharedPtrTest
    : public testing::Test
{
public:
    testing::StrictMock<ObjectConstructionMock> objectConstructionMock;
    infra::SharedObjectAllocatorFixedSize<MySharedObject, void(ObjectConstructionMock&)>::WithStorage<2> allocator;
};

TEST_F(SharedPtrTest, allocate_one_object)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);
    EXPECT_TRUE(static_cast<bool>(object));
    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, when_allocation_fails_empty_SharedPtr_is_returned)
{
    infra::SharedObjectAllocatorFixedSize<MySharedObject, void(ObjectConstructionMock&)>::WithStorage<0> allocator;

    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);
    EXPECT_FALSE(static_cast<bool>(object));
}

TEST_F(SharedPtrTest, share_one_object_by_copy_construction)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);

    {
        infra::SharedPtr<MySharedObject> object2(object);
        EXPECT_TRUE(static_cast<bool>(object2));
    }

    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, share_one_object_by_assignment)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);

    {
        infra::SharedPtr<MySharedObject> object2;
        object2 = object;
        EXPECT_TRUE(static_cast<bool>(object2));
    }

    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, reset_SharedPtr_by_assigning_nullptr)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);

    EXPECT_CALL(objectConstructionMock, Destruct());
    object = nullptr;
    testing::Mock::VerifyAndClearExpectations(&objectConstructionMock);
}

TEST_F(SharedPtrTest, move_construct_SharedPtr)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);

    infra::SharedPtr<MySharedObject> object2(std::move(object));
    EXPECT_FALSE(static_cast<bool>(object));

    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, move_copy_SharedPtr)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);

    infra::SharedPtr<MySharedObject> object2;
    object2 = std::move(object);
    EXPECT_FALSE(static_cast<bool>(object));

    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, convert_SharedPtr_to_and_from_const)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);

    infra::SharedPtr<const MySharedObject> constObject(object);
    EXPECT_TRUE(static_cast<bool>(constObject));
    infra::SharedPtr<const MySharedObject> movedConstObject(std::move(object));
    EXPECT_FALSE(static_cast<bool>(object));
    EXPECT_TRUE(static_cast<bool>(movedConstObject));

    infra::SharedPtr<MySharedObject> nonConstObject(infra::ConstCast(constObject));
    EXPECT_TRUE(static_cast<bool>(nonConstObject));
    infra::SharedPtr<MySharedObject> movedNonConstObject(infra::ConstCast(std::move(constObject)));
    EXPECT_FALSE(static_cast<bool>(constObject));
    EXPECT_TRUE(static_cast<bool>(movedNonConstObject));

    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, convert_SharedPtr_to_and_from_base)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);

    infra::SharedPtr<MySharedObjectBase> baseObject(object);
    EXPECT_TRUE(static_cast<bool>(baseObject));
    infra::SharedPtr<MySharedObjectBase> movedBaseObject(std::move(object));
    EXPECT_FALSE(static_cast<bool>(object));
    EXPECT_TRUE(static_cast<bool>(movedBaseObject));

    infra::SharedPtr<MySharedObject> derivedObject(infra::StaticPointerCast<MySharedObject>(baseObject));
    EXPECT_TRUE(static_cast<bool>(derivedObject));
    infra::SharedPtr<MySharedObject> movedDerivedObject(infra::StaticPointerCast<MySharedObject>(std::move(baseObject)));
    EXPECT_FALSE(static_cast<bool>(baseObject));
    EXPECT_TRUE(static_cast<bool>(movedDerivedObject));

    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, dereference_SharedPtr)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    const infra::SharedPtr<const MySharedObject> object = allocator.Allocate(objectConstructionMock);

    EXPECT_EQ(5, object->Value());
    EXPECT_EQ(5, (*object).Value());

    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, test_equality)
{
    EXPECT_CALL(objectConstructionMock, Construct()).Times(2);
    infra::SharedPtr<MySharedObject> object1 = allocator.Allocate(objectConstructionMock);
    infra::SharedPtr<MySharedObject> object2 = allocator.Allocate(objectConstructionMock);
    infra::SharedPtr<MySharedObject> objectEmpty;

    infra::SharedPtr<MySharedObject> object1Same = object1;

    EXPECT_TRUE(object1 == object1Same);
    EXPECT_FALSE(object1 != object1Same);
    EXPECT_FALSE(object1 == object2);
    EXPECT_TRUE(object1 != object2);
    EXPECT_FALSE(object1 == objectEmpty);
    EXPECT_TRUE(object1 != objectEmpty);

    EXPECT_FALSE(object1 == nullptr);
    EXPECT_FALSE(nullptr == object1);
    EXPECT_TRUE(object1 != nullptr);
    EXPECT_TRUE(nullptr != object1);

    EXPECT_TRUE(objectEmpty == nullptr);
    EXPECT_TRUE(nullptr == objectEmpty);
    EXPECT_FALSE(objectEmpty != nullptr);
    EXPECT_FALSE(nullptr != objectEmpty);

    EXPECT_CALL(objectConstructionMock, Destruct()).Times(2);
}

TEST_F(SharedPtrTest, construct_WeakPtr)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);
    infra::WeakPtr<MySharedObject> weakObject(object);
    infra::WeakPtr<MySharedObject> weakObject2 = object;
    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, construct_SharedPtr_from_WeakPtr)
{
    infra::SharedPtr<MySharedObject> sharedObject;
    {
        EXPECT_CALL(objectConstructionMock, Construct());
        infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);
        infra::WeakPtr<MySharedObject> weakObject(object);
        sharedObject = weakObject.lock();
    }

    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, convert_WeakPtr_to_SharedPtr)
{
    EXPECT_CALL(objectConstructionMock, Construct());
    infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);
    infra::WeakPtr<MySharedObject> weakObject(object);
    infra::SharedPtr<MySharedObject> sharedObject(weakObject);
    infra::SharedPtr<MySharedObject> sharedObject2;
    sharedObject2 = weakObject;
    EXPECT_CALL(objectConstructionMock, Destruct());
}

TEST_F(SharedPtrTest, object_is_destructed_but_not_deallocated_while_WeakPtr_has_a_reference)
{
    infra::SharedObjectAllocatorFixedSize<MySharedObject, void(ObjectConstructionMock&)>::WithStorage<1> allocator;

    infra::WeakPtr<MySharedObject> weakObject;

    {
        EXPECT_CALL(objectConstructionMock, Construct());
        infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);
        weakObject = object;
        EXPECT_CALL(objectConstructionMock, Destruct());
    }
    testing::Mock::VerifyAndClearExpectations(&objectConstructionMock);

    EXPECT_EQ(nullptr, allocator.Allocate(objectConstructionMock));
}

TEST_F(SharedPtrTest, move_construct_WeakPtr)
{
    infra::SharedObjectAllocatorFixedSize<MySharedObject, void(ObjectConstructionMock&)>::WithStorage<1> allocator;

    infra::WeakPtr<MySharedObject> weakObject;

    {
        EXPECT_CALL(objectConstructionMock, Construct());
        infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);
        weakObject = object;
        EXPECT_CALL(objectConstructionMock, Destruct());

        infra::WeakPtr<MySharedObject> weakObject2(std::move(weakObject));
    }
    EXPECT_CALL(objectConstructionMock, Construct());
    EXPECT_CALL(objectConstructionMock, Destruct());
    EXPECT_NE(nullptr, allocator.Allocate(objectConstructionMock));
}

TEST_F(SharedPtrTest, move_assign_WeakPtr)
{
    infra::SharedObjectAllocatorFixedSize<MySharedObject, void(ObjectConstructionMock&)>::WithStorage<1> allocator;

    infra::WeakPtr<MySharedObject> weakObject;

    {
        EXPECT_CALL(objectConstructionMock, Construct());
        infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);
        weakObject = object;
        EXPECT_CALL(objectConstructionMock, Destruct());

        infra::WeakPtr<MySharedObject> weakObject2;
        weakObject2 = std::move(weakObject);
    }
    EXPECT_CALL(objectConstructionMock, Construct());
    EXPECT_CALL(objectConstructionMock, Destruct());
    EXPECT_NE(nullptr, allocator.Allocate(objectConstructionMock));
}

TEST_F(SharedPtrTest, converting_WeakPtr_to_SharedPtr_for_expired_object_results_in_nullptr)
{
    infra::SharedObjectAllocatorFixedSize<MySharedObject, void(ObjectConstructionMock&)>::WithStorage<1> allocator;

    infra::WeakPtr<MySharedObject> weakObject;

    {
        EXPECT_CALL(objectConstructionMock, Construct());
        infra::SharedPtr<MySharedObject> object = allocator.Allocate(objectConstructionMock);
        weakObject = object;
        EXPECT_CALL(objectConstructionMock, Destruct());
    }

    infra::SharedPtr<MySharedObject> object(weakObject);
    EXPECT_EQ(nullptr, object);
}
