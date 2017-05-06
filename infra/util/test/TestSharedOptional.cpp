#include <gmock/gmock.h>
#include "infra/util/public/SharedOptional.hpp"

TEST(SharedOptionalTest, Emplace_returns_SharedPtr)
{
    infra::SharedOptional<int> s;
    EXPECT_FALSE(static_cast<bool>(s));
    EXPECT_TRUE(!s);
    infra::SharedPtr<int> p = s.Emplace(5);
    EXPECT_EQ(5, *p);
    EXPECT_TRUE(static_cast<bool>(s));
    EXPECT_FALSE(!s);
}

TEST(SharedOptionalTest, after_Emplace_SharedOptional_is_not_allocatable)
{
    infra::SharedOptional<int> s;
    EXPECT_TRUE(s.Allocatable());
    infra::SharedPtr<int> p = s.Emplace(5);
    EXPECT_FALSE(s.Allocatable());
}

TEST(SharedOptionalTest, after_Emplace_SharedOptional_is_deferenceable)
{
    infra::SharedOptional<int> s;
    const infra::SharedOptional<int>& cs = s;
    infra::SharedPtr<int> p = s.Emplace(5);
    EXPECT_EQ(5, *s);
    EXPECT_EQ(5, *cs);
    EXPECT_EQ(5, *(s.operator->()));
    EXPECT_EQ(5, *(cs.operator->()));
}

TEST(SharedOptionalTest, after_SharedPtr_destruction_SharedOptional_is_not_dereferenceable)
{
    infra::SharedOptional<int> s;
    infra::SharedPtr<int> p = s.Emplace(5);
    p = nullptr;
    EXPECT_FALSE(static_cast<bool>(s));
}

TEST(SharedOptionalTest, only_after_WeakPtr_destruction_SharedOptional_is_allocatable)
{
    infra::SharedOptional<int> s;
    infra::SharedPtr<int> p = s.Emplace(5);
    infra::WeakPtr<int> wp = p;
    p = nullptr;
    EXPECT_FALSE(s.Allocatable());
    wp = nullptr;
    EXPECT_TRUE(s.Allocatable());
}
