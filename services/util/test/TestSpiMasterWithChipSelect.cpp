#include "gtest/gtest.h"
#include "hal/interfaces/test_doubles/public/CommunicationConfiguratorMock.hpp"
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
    {
        EXPECT_CALL(spiMock, SetChipSelectConfiguratorMock(testing::_));
        spi.Emplace(spiMock, chipSelect);
    }

    testing::StrictMock<hal::SpiAsynchronousMock> spiMock;
    hal::GpioPinStub chipSelect;
    infra::Optional<services::SpiMasterWithChipSelect> spi;
};

TEST_F(SpiMasterWithChipSelectTest, ChipSelectStartsHigh)
{
    EXPECT_TRUE(chipSelect.GetStubState());
}

TEST_F(SpiMasterWithChipSelectTest, on_StartSession_chip_select_is_activated)
{
    EXPECT_CALL(spiMock, SendAndReceiveMock(testing::_, testing::_)).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 1 })));

    std::array<uint8_t, 1> buffer;
    spi->StartSession();
    spi->SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);

    EXPECT_FALSE(chipSelect.GetStubState());
}

TEST_F(SpiMasterWithChipSelectTest, on_EndSession_chip_select_is_deactivated)
{
    EXPECT_CALL(spiMock, SendAndReceiveMock(testing::_, testing::_)).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 1 })));

    std::array<uint8_t, 1> buffer;
    spi->StartSession();
    spi->SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    spi->EndSession();

    EXPECT_TRUE(chipSelect.GetStubState());
}

TEST_F(SpiMasterWithChipSelectTest, additional_ChipSelectConfigurator_is_invoked)
{
    hal::ChipSelectConfiguratorMock chipSelectConfiguratorMock;
    spi->SetChipSelectConfigurator(chipSelectConfiguratorMock);

    EXPECT_CALL(spiMock, SendAndReceiveMock(testing::_, testing::_)).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 1 })));

    std::array<uint8_t, 1> buffer;
    EXPECT_CALL(chipSelectConfiguratorMock, StartSession());
    spi->StartSession();
    spi->SendAndReceive(buffer, buffer, hal::SpiAction::stop, infra::emptyFunction);
    EXPECT_CALL(chipSelectConfiguratorMock, EndSession());
    spi->EndSession();
}

TEST_F(SpiMasterWithChipSelectTest, SetCommunicationConfigurator_is_forwarded)
{
    hal::CommunicationConfiguratorMock configurator;

    EXPECT_CALL(spiMock, SetCommunicationConfigurator(testing::Ref(configurator)));
    spi->SetCommunicationConfigurator(configurator);

    EXPECT_CALL(spiMock, ResetCommunicationConfigurator());
    spi->ResetCommunicationConfigurator();
}
