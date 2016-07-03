#include "gmock/gmock.h"
#include "infra/util/public/ProxyCreator.hpp"

class PeripheralInterface
{
public:
    virtual void Send() = 0;
};

class Peripheral
    : public PeripheralInterface
{
public:
    void Send()
    {
        SendMock();
    }

    MOCK_METHOD0(SendMock, void());
};

class PeripheralWithTwoParameters
    : public PeripheralInterface
{
public:
    PeripheralWithTwoParameters(int x, int y)
        : x(x)
        , y(y)
    {}

    void Send()
    {
        SendMock();
    }

    MOCK_METHOD0(SendMock, void());
    int x;
    int y;
};

TEST(ProxyCreatorTest, CreatePeripheral)
{
    infra::Creator<PeripheralInterface, Peripheral> creator;

    infra::ProxyCreator<PeripheralInterface> creatorProxy(creator);
}

TEST(ProxyCreatorTest, CreatePeripheralWithParameterGivenByProxy)
{
    infra::Creator<PeripheralInterface, PeripheralWithTwoParameters, int, int> creator;

    infra::ProxyCreator<PeripheralInterface, int, int> creatorProxy(creator, 5, 6);
    EXPECT_EQ(5, creator->x);
}

TEST(ProxyCreatorTest, CreatePeripheralWithParameterGivenByCreator)
{
    infra::Creator<PeripheralInterface, PeripheralWithTwoParameters> creator([](infra::Optional<PeripheralWithTwoParameters>& object) { object.Emplace(5, 6); });

    infra::ProxyCreator<PeripheralInterface> creatorProxy(creator);
    EXPECT_EQ(5, creator->x);
}

TEST(ProxyCreatorTest, CreatePeripheralWithParameterGivenByProxyAndCreator)
{
    infra::Creator<PeripheralInterface, PeripheralWithTwoParameters, int> creator([](infra::Optional<PeripheralWithTwoParameters>& object, int x) { object.Emplace(x, 6); });

    infra::ProxyCreator<PeripheralInterface, int> creatorProxy(creator, 5);
    EXPECT_EQ(5, creator->x);
    EXPECT_EQ(6, creator->y);
}

TEST(ProxyCreatorTest, CreatePeripheralWithParameterGivenByCreatorAndProxy)
{
    infra::Creator<PeripheralInterface, PeripheralWithTwoParameters, int> creator([](infra::Optional<PeripheralWithTwoParameters>& object, int y) { object.Emplace(5, y); });

    infra::ProxyCreator<PeripheralInterface, int> creatorProxy(creator, 6);
    EXPECT_EQ(5, creator->x);
    EXPECT_EQ(6, creator->y);
}

TEST(ProxyCreatorTest, AccessPeripheral)
{
    infra::Creator<PeripheralInterface, Peripheral> creator;

    infra::ProxyCreator<PeripheralInterface> creatorProxy(creator);

    EXPECT_CALL(*creator, SendMock());
    creatorProxy->Send();
}
