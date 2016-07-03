#include "gtest/gtest.h"
#include "infra/util/public/InterfaceConnector.hpp"

class ITestSingleton
    : public infra::InterfaceConnector<ITestSingleton>
{
public:
    virtual void f() = 0;
};

class TestSingleton
    : public ITestSingleton
{
public:
    virtual void f() override {}
};

TEST(InterfaceConnectorTest, TestConstruction)
{
    TestSingleton t;

    EXPECT_EQ(&t, &ITestSingleton::Instance());
}
