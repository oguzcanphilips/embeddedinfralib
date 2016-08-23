#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/util/public/Function.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"

TEST(FunctionTest, TestConstructedEmpty)
{
    infra::Function<void()> f;
    EXPECT_FALSE(f);
}

TEST(FunctionTest, TestConstructedEmptyByNullptr)
{
    infra::Function<void()> f(nullptr);
    EXPECT_FALSE(f);
}

TEST(FunctionTest, TestConstructedNotEmpty)
{
    infra::Function<void()> f([]() {} );
    EXPECT_TRUE(static_cast<bool>(f));
}

TEST(FunctionTest, TestConstructedWithTheEmptyFunction)
{
    infra::Function<void()> f(infra::emptyFunction);
    EXPECT_TRUE(static_cast<bool>(f));
    f();
}

TEST(FunctionTest, TestCall)
{
    infra::MockCallback<void()> m;
    infra::Function<void()> f([&m]() { m.callback(); } );
    EXPECT_CALL(m, callback());
    f();
}

TEST(FunctionTest, TestInvoke0)
{
    infra::MockCallback<void()> m;
    infra::Function<void()> f([&m]() { m.callback(); });
    EXPECT_CALL(m, callback());
    f.Invoke(std::make_tuple());
}

TEST(FunctionTest, TestInvoke1)
{
    infra::MockCallback<void(int)> m;
    infra::Function<void(int)> f([&m](int x) { m.callback(x); });
    EXPECT_CALL(m, callback(1));
    f.Invoke(std::make_tuple(1));
}

TEST(FunctionTest, TestInvoke2)
{
    infra::MockCallback<void(int, int)> m;
    infra::Function<void(int, int)> f([&m](int x, int y) { m.callback(x, y); });
    EXPECT_CALL(m, callback(1, 2));
    f.Invoke(std::make_tuple(1, 2));
}

TEST(FunctionTest, TestInvoke3)
{
    infra::MockCallback<void(int, int, int)> m;
    infra::Function<void(int, int, int)> f([&m](int x, int y, int z) { m.callback(x, y, z); });
    EXPECT_CALL(m, callback(1, 2, 3));
    f.Invoke(std::make_tuple(1, 2, 3));
}

TEST(FunctionTest, TestCopyConstruct)
{
    infra::MockCallback<void()> m;
    infra::Function<void()> f([&m]() { m.callback(); } );
    infra::Function<void()> f2(f);
    EXPECT_TRUE(static_cast<bool>(f));
    EXPECT_CALL(m, callback());
    f2();
}

TEST(FunctionTest, TestCopyConstructTwice)
{
    infra::MockCallback<void()> m;
    infra::Function<void()> f([&m]() { m.callback(); });
    infra::Function<void()> f2(f);
    infra::Function<void()> f3(f2);
    EXPECT_TRUE(static_cast<bool>(f));
    EXPECT_CALL(m, callback());
    f3();
}

TEST(FunctionTest, TestCopyAssign)
{
    infra::MockCallback<void()> m;
    infra::Function<void()> f([&m]() { m.callback(); } );
    infra::Function<void()> f2;
    f2 = f;
    EXPECT_TRUE(static_cast<bool>(f));
    EXPECT_CALL(m, callback());
    f2();
}

TEST(FunctionTest, TestAssignNullptr)
{
    infra::MockCallback<void()> m;
    infra::Function<void()> f([&m]() { m.callback(); } );
    f = nullptr;
    EXPECT_FALSE(static_cast<bool>(f));
}

TEST(FunctionTest, TestAssign)
{
    infra::MockCallback<void()> m;
    infra::Function<void()> f;
    f = [&m]() { m.callback(); };
    EXPECT_CALL(m, callback());
    f();
}

TEST(FunctionTest, TestSwap)
{
    infra::MockCallback<void()> m;
    infra::Function<void()> f([&m]() { m.callback(); } );
    infra::Function<void()> f2;
    swap(f, f2);
    EXPECT_FALSE(static_cast<bool>(f));
    EXPECT_CALL(m, callback());
    f2();
}

TEST(FunctionTest, TestReturnValue)
{
    infra::Function<int()> f([]() { return 1; } );
    EXPECT_EQ(1, f());
}

TEST(FunctionTest, TestParameter)
{
    infra::MockCallback<void(int)> m;
    infra::Function<void(int)> f([&m](int x) { m.callback(x); } );
    EXPECT_CALL(m, callback(1));
    f(1);
}

TEST(FunctionProxyTest, TestProxy)
{
    infra::MockCallback<void()> m;
    infra::Function<void(), 40> p([&m]() { m.callback(); } );

    EXPECT_CALL(m, callback());
    infra::Function<void(), 4> f([&p]() { p(); });
    f();
}

TEST(FunctionTest, TestCompareToNullptr)
{
    infra::Function<void()> f;

    EXPECT_EQ(f, nullptr);
    EXPECT_EQ(nullptr, f);

    infra::Function<void()> g([]() {});

    EXPECT_NE(g, nullptr);
    EXPECT_NE(nullptr, g);
}
