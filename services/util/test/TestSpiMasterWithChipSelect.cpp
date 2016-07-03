#include "gtest/gtest.h"
#include "hal/interfaces/test_doubles/public/GpioStub.hpp"
#include "hal/interfaces/test_doubles/public/SpiMock.hpp"
#include "infra/event/test_helper/public/EventDispatcherFixture.hpp"
#include "services/util/public/SpiMasterWithChipSelect.hpp"

class SpiMasterWithChipSelectTest
    : public testing::Test
    , public infra::EventDispatcherFixture
{
public:
    SpiMasterWithChipSelectTest()
        : spi(spiMock, chipSelect)
    {}

    testing::StrictMock<hal::SpiAsynchronousMock> spiMock;
    hal::GpioPinStub chipSelect;
    services::SpiMasterWithChipSelect spi;
};

TEST_F(SpiMasterWithChipSelectTest, ChipSelectStartsHigh)
{
    EXPECT_TRUE(chipSelect.GetStubState());
}

TEST_F(SpiMasterWithChipSelectTest, OnSendAndReceiveScheduleChipSelectIsNotYetActivated)
{
    EXPECT_CALL(spiMock, SendAndReceiveMock(testing::_, testing::_)).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{})));

    std::array<uint8_t, 1> buffer;
    spi.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);

    EXPECT_TRUE(chipSelect.GetStubState());
}

TEST_F(SpiMasterWithChipSelectTest, WhenSendAndReceivesStartsChipSelectIsActivated)
{
    EXPECT_CALL(spiMock, SendAndReceiveMock(testing::_, testing::_)).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 1 })));

    std::array<uint8_t, 1> buffer;
    spi.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);

    EXPECT_FALSE(chipSelect.GetStubState());
}

TEST_F(SpiMasterWithChipSelectTest, WhenSendAndReceivesFinishesChipSelectIsDeactivated)
{
    EXPECT_CALL(spiMock, SendAndReceiveMock(testing::_, testing::_)).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 1 })));

    std::array<uint8_t, 1> buffer;
    spi.SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    ExecuteAllActions();

    EXPECT_TRUE(chipSelect.GetStubState());
}
