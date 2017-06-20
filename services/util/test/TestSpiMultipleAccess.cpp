#include "gtest/gtest.h"
#include "hal/interfaces/test_doubles/CommunicationConfiguratorMock.hpp"
#include "hal/interfaces/test_doubles/SpiMock.hpp"
#include "infra/event/test_helper/EventDispatcherFixture.hpp"
#include "services/util/SpiMultipleAccess.hpp"

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

    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
    spi.chipSelectConfigurator->StartSession();
}

TEST_F(SpiMultipleAccessTest, SecondSendAndReceiveIsNotExecuted)
{
    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    access2.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, SecondSendAndReceiveIsExecutedWhenFirstAccessFinishes)
{
    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    access2.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();

    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    spi.chipSelectConfigurator->StartSession();
    spi.onDone();
    spi.chipSelectConfigurator->EndSession();

    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, AfterSendAndReceiveWithContinueSessionClaimIsNotReleased)
{
    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::continueSession)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::continueSession, infra::emptyFunction);
    access2.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();

    spi.chipSelectConfigurator->StartSession();
    spi.onDone();
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, ReceiveStopAfterContinueSessionReleasesTheClaim)
{
    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::continueSession)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::continueSession, infra::emptyFunction);
    access2.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();

    spi.chipSelectConfigurator->StartSession();
    spi.onDone();
    ExecuteAllActions();

    testing::Mock::VerifyAndClearExpectations(&spi);

    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_)).Times(2);
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 }))).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
    spi.onDone();
    spi.chipSelectConfigurator->EndSession();
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, additional_ChipSelectConfigurator_is_invoked)
{
    hal::ChipSelectConfiguratorMock chipSelectConfiguratorMock;
    access1.SetChipSelectConfigurator(chipSelectConfiguratorMock);

    std::array<uint8_t, 1> buffer;

    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
    EXPECT_CALL(chipSelectConfiguratorMock, StartSession());
    spi.chipSelectConfigurator->StartSession();
    spi.onDone();
    EXPECT_CALL(chipSelectConfiguratorMock, EndSession());
    spi.chipSelectConfigurator->EndSession();
}

TEST_F(SpiMultipleAccessTest, configurator_is_set_when_first_session_starts)
{
    std::array<uint8_t, 1> buffer;

    testing::StrictMock<hal::CommunicationConfiguratorMock> configurator;
    access1.SetCommunicationConfigurator(configurator);

    EXPECT_CALL(spi, SetCommunicationConfigurator(testing::Ref(configurator)));
    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
    spi.chipSelectConfigurator->StartSession();

    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
}

TEST_F(SpiMultipleAccessTest, configurator_is_reset_when_other_session_starts)
{
    std::array<uint8_t, 1> buffer;

    testing::StrictMock<hal::CommunicationConfiguratorMock> configurator;
    access1.SetCommunicationConfigurator(configurator);

    EXPECT_CALL(spi, SetCommunicationConfigurator(testing::Ref(configurator)));
    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access1.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
    spi.chipSelectConfigurator->StartSession();
    spi.onDone();
    spi.chipSelectConfigurator->EndSession();

    EXPECT_CALL(spi, ResetCommunicationConfigurator());
    EXPECT_CALL(spi, SetChipSelectConfiguratorMock(testing::_));
    EXPECT_CALL(spi, SendAndReceiveMock(testing::_, hal::SpiAction::stop)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 5 })));
    access2.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();
    spi.chipSelectConfigurator->StartSession();
}
