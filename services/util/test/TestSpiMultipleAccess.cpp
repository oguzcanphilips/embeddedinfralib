#include "gtest/gtest.h"
#include "hal/interfaces/test_doubles/public/SpiMock.hpp"
#include "infra/event/test_helper/public/EventDispatcherFixture.hpp"
#include "services/util/public/SpiMultipleAccess.hpp"

class SpiMultipleAccessTest
    : public testing::Test
    , public infra::EventDispatcherFixture
{
public:
    SpiMultipleAccessTest()
        : multipleAccess(spi)
        , access1(multipleAccess)
        , access2(multipleAccess)
    {}

    testing::StrictMock<hal::SpiAsynchronousMock> spi;
    services::SpiMultipleAccessMaster multipleAccess;
    services::SpiMultipleAccess access1;
    services::SpiMultipleAccess access2;
};

TEST_F(SpiMultipleAccessTest, FirstSendAndReceiveIsExecuted)
{
    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, SecondSendAndReceiveIsNotExecuted)
{
    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    access2.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, SecondSendAndReceiveIsExecutedWhenFirstAccessFinishes)
{
    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    access2.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();

    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    spi.actionOnCompletion();
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, AfterSendAndReceiveWithContinueSessionClaimIsNotReleased)
{
    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::continueSession)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::continueSession, infra::emptyFunction);
    access2.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();

    spi.actionOnCompletion();
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, ReceiveStopAfterContinueSessionReleasesTheClaim)
{
    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::continueSession)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::continueSession, infra::emptyFunction);
    access2.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();

    spi.actionOnCompletion();
    ExecuteAllActions();

    testing::Mock::VerifyAndClearExpectations(&spi);

    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 }))).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
    spi.actionOnCompletion();
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, DefaultSpeedAndModeAreConfiguredBeforeSendAndReceive)
{
    EXPECT_CALL(spi, ConfigSpeedMock(100));
    EXPECT_CALL(spi, ConfigModeMock(0));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 5 })));
    std::array<uint8_t, 1> buffer = { 5 };
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, ConfiguredSpeedAndModeAreSetBeforeSendAndReceive)
{
    EXPECT_CALL(spi, ConfigSpeedMock(50));
    EXPECT_CALL(spi, ConfigModeMock(3));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 5 })));
    access1.ConfigSpeed(50);
    access1.ConfigMode(3);
    std::array<uint8_t, 1> buffer = { 5 };
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, ConfiguredSpeedAndModeAreNotSetBeforeSendAndReceiveStarts)
{
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.ConfigSpeed(100);
    access1.ConfigMode(3);
    std::array<uint8_t, 1> buffer = { 5 };
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
}
