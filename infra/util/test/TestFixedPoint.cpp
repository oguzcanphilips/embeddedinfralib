#include "gtest/gtest.h"
#include "infra/util/FixedPoint.hpp"

TEST(FixedPointTest, CreateYieldsZero)
{
    infra::FixedPoint<int, 10> f;
    EXPECT_EQ(0, f.AsUnscaled());
}

TEST(FixedPointTest, CreationWithUnscaledValue)
{
    infra::FixedPoint<int, 10> f(infra::unscaled, 34);
    EXPECT_EQ(34, f.AsUnscaled());
}

TEST(FixedPointTest, CreationWithScaledValue)
{
    infra::FixedPoint<int, 10> f(infra::scaled, 34);
    EXPECT_EQ(340, f.AsUnscaled());
}

TEST(FixedPointTest, IntegerPart)
{
    infra::FixedPoint<int, 10> f(infra::unscaled, 34);
    EXPECT_EQ(3, f.IntegerPart());
}

TEST(FixedPointTest, FractionalPart)
{
    infra::FixedPoint<int, 10> f(infra::unscaled, 34);
    EXPECT_EQ(4, f.FractionalPart());
}

TEST(FixedPointTest, Addition)
{
    infra::FixedPoint<int, 10> f1(infra::unscaled, 34);
    infra::FixedPoint<int, 10> f2(infra::unscaled, 15);

    EXPECT_EQ(49, (f1 + f2).AsUnscaled());

    f1 += f2;
    EXPECT_EQ(49, f1.AsUnscaled());
}

TEST(FixedPointTest, Subtraction)
{
    infra::FixedPoint<int, 10> f1(infra::unscaled, 34);
    infra::FixedPoint<int, 10> f2(infra::unscaled, 15);

    EXPECT_EQ(19, (f1 - f2).AsUnscaled());

    f1 -= f2;
    EXPECT_EQ(19, f1.AsUnscaled());
}

TEST(FixedPointTest, Multiplication)
{
    infra::FixedPoint<int, 10> f1(infra::unscaled, 34);
    infra::FixedPoint<int, 10> f2(infra::unscaled, 15);

    EXPECT_EQ(51, (f1 * f2).AsUnscaled());
    //EXPECT_EQ(64, (f1 * 2).AsUnscaled());
    //EXPECT_EQ(64, (2 * f1).AsUnscaled());

    f1 *= f2;
    EXPECT_EQ(51, f1.AsUnscaled());
    f1 *= 2;
    EXPECT_EQ(102, f1.AsUnscaled());
}

TEST(FixedPointTest, Division)
{
    infra::FixedPoint<int, 10> f1(infra::unscaled, 34);
    infra::FixedPoint<int, 10> f2(infra::unscaled, 15);

    EXPECT_EQ(22, (f1 / f2).AsUnscaled());

    f1 /= f2;
    EXPECT_EQ(22, f1.AsUnscaled());
    f1 /= 2;
    EXPECT_EQ(11, f1.AsUnscaled());
}

TEST(FixedPointTest, Negation)
{
    infra::FixedPoint<int, 10> f1(infra::unscaled, 34);
    EXPECT_EQ(-34, (-f1).AsUnscaled());
}

TEST(FixedPointTest, Comparisons)
{
    infra::FixedPoint<int, 10> f1(infra::unscaled, 30);
    infra::FixedPoint<int, 10> f2(infra::unscaled, 15);

    EXPECT_TRUE(f1 == f1);
    EXPECT_FALSE(f1 == f2);

    EXPECT_TRUE(f2 < f1);
    EXPECT_FALSE(f1 < f2);
    EXPECT_FALSE(f1 < f1);

    EXPECT_TRUE(f1 == 3);
    EXPECT_FALSE(f2 == 1);
    EXPECT_FALSE(f2 == 2);

    EXPECT_TRUE(f1 < 4);
    EXPECT_FALSE(f1 < 3);
    EXPECT_TRUE(f2 < 2);
    EXPECT_FALSE(f2 < 1);

    EXPECT_TRUE(f1 > 2);
    EXPECT_FALSE(f1 > 3);
    EXPECT_TRUE(f2 > 1);
    EXPECT_FALSE(f2 > 2);
}
