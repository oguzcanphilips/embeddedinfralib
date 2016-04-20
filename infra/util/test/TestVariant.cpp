#include "gtest/gtest.h"
#include "infra/util/public/Variant.hpp"
#include <cstdint>

TEST(VariantTest, TestEmptyConstruction)
{
    infra::Variant<bool> v;
}

TEST(VariantTest, TestConstructionWithBool)
{
    bool b;
    infra::Variant<bool> v(b);
}

TEST(VariantTest, TestConstructionWithVariant)
{
    infra::Variant<bool, int> i(5);
    infra::Variant<bool, int> v(i);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestConstructionWithNarrowVariant)
{
    infra::Variant<int> i(5);
    infra::Variant<bool, int> v(i);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestConstructionAtIndex)
{
    infra::Variant<uint8_t, uint16_t> v(infra::atIndex, 1, 3);
    EXPECT_EQ(1, v.Which());
    EXPECT_EQ(3, v.Get<uint16_t>());
}

TEST(VariantTest, TestGetBool)
{
    bool b = true;
    infra::Variant<bool> v(b);
    EXPECT_TRUE(v.Get<bool>());
}

TEST(VariantTest, TestAssignment)
{
    infra::Variant<bool, int> v(true);
    v = 5;
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestInPlaceConstruction)
{
    struct MyStruct
    {
        MyStruct(int aX, int aY): x(aX), y(aY) {};

        int x;
        int y;
    };

    infra::Variant<MyStruct> v(infra::InPlaceType<MyStruct>(), 2, 3);
    EXPECT_EQ(0, v.Which());
    EXPECT_EQ(2, v.Get<MyStruct>().x);
    EXPECT_EQ(3, v.Get<MyStruct>().y);
}

TEST(VariantTest, TestEmplace)
{
    struct MyStruct
    {
        MyStruct(int aX, int aY): x(aX), y(aY) {};

        int x;
        int y;
    };

    infra::Variant<bool, MyStruct> v(true);
    v.Emplace<MyStruct>(2, 3);
    EXPECT_EQ(1, v.Which());
    EXPECT_EQ(2, v.Get<MyStruct>().x);
    EXPECT_EQ(3, v.Get<MyStruct>().y);
}

TEST(VariantTest, TestAssignmentFromVariant)
{
    infra::Variant<bool, int> v(true);
    v = infra::Variant<bool, int>(5);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestAssignmentFromNarrowVariant)
{
    infra::Variant<bool, int> v(true);
    v = infra::Variant<int>(5);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestVariantWithTwoTypes)
{
    int i = 5;
    infra::Variant<bool, int> v(i);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestVisitor)
{
    struct Visitor
        : infra::StaticVisitor<void>
    {
        Visitor()
            : passed(0)
        {}

        void operator()(bool b)
        {}

        void operator()(int i)
        {
            ++passed;
        }

        int passed;
    };

    int i = 5;
    infra::Variant<bool, int> variant(i);
    Visitor visitor;
    infra::ApplyVisitor(visitor, variant);
    EXPECT_EQ(1, visitor.passed);
}

TEST(VariantTest, TestReturningVisitor)
{
    struct Visitor
        : infra::StaticVisitor<bool>
    {
        bool operator()(bool b)
        {
            return false;
        }

        bool operator()(int i)
        {
            return true;
        }
    };

    int i = 5;
    infra::Variant<bool, int> variant(i);
    Visitor visitor;
    EXPECT_EQ(true, infra::ApplyVisitor(visitor, variant));
}

TEST(VariantTest, TestEqual)
{
    int i = 1;
    int j = 2;
    bool k = true;
    const infra::Variant<bool, int> v1(i);
    const infra::Variant<bool, int> v2(i);
    const infra::Variant<bool, int> v3(j);
    const infra::Variant<bool, int> v4(k);

    EXPECT_EQ(v1, v2);
    EXPECT_NE(v1, v3);
    EXPECT_NE(v1, v4);
}

TEST(VariantTest, TestLessThan)
{
    int i = 1;
    int j = 2;
    bool k = true;
    const infra::Variant<bool, int> v1(i);
    const infra::Variant<bool, int> v2(i);
    const infra::Variant<bool, int> v3(j);
    const infra::Variant<bool, int> v4(k);

    EXPECT_GE(v1, v2);
    EXPECT_LE(v1, v2);
    EXPECT_LT(v1, v3);
    EXPECT_GT(v1, v4);
}

struct DoubleVisitor
    : infra::StaticVisitor<int>
{
    template<class T, class U>
    int operator()(T x, U y)
    {
        return x + y;
    }
};

TEST(VariantTest, TestDoubleVisitor)
{
    infra::Variant<bool, int> variant1(5);
    infra::Variant<bool, int> variant2(true);
    DoubleVisitor visitor;
    EXPECT_EQ(6, infra::ApplyVisitor(visitor, variant1, variant2));
}

struct EmptyVisitor
    : infra::StaticVisitor<void>
{
    template<class T>
    void operator()(T)
    {}

    template<class T>
    void operator()(T, T)
    {}

    template<class T, class U>
    void operator()(T, U)
    {}
};

TEST(VariantTest, TestRecursiveLoopUnrolling)
{
    struct A {};
    struct B {};
    struct C {};
    struct D {};
    struct E {};
    struct F {};

    infra::Variant<A, B, C, D, E, F> v((F()));
    EmptyVisitor visitor;
    infra::ApplyVisitor(visitor, v);
    infra::ApplyVisitor(visitor, v, v);
    infra::ApplySameTypeVisitor(visitor, v, v);    
}
