#include "gmock/gmock.h"
#include "hal/interfaces/test_doubles/public/GpioStub.hpp"
#include "infra/timer/test_helper/public/ClockFixture.hpp"
#include "infra/stream/public/ByteInputStream.hpp"
#include "infra/stream/public/ByteOutputStream.hpp"
#include "infra/util/public/Optional.hpp"
#include "hal/interfaces/test_doubles/public/FlashStub.hpp"
#include "packs/upgrade/deploy_pack_to_external/public/DeployPackToExternal.hpp"
#include "packs/upgrade/pack/public/UpgradePackHeader.hpp"

class DeployPackToExternalTest
    : public testing::Test
    , public infra::ClockFixture
{
public:
    DeployPackToExternalTest()
        : from(1, 4096)
        , to(1, 4096)
    {}

    hal::FlashStub from;
    hal::FlashStub to;
    hal::GpioPinSpy statusLed;
};

TEST_F(DeployPackToExternalTest, DeployPackCopiesHeader)
{
    infra::ByteOutputStream outputStream(from.sectors[0]);
    application::UpgradePackHeaderPrologue header = {};
    header.status = application::UpgradePackStatus::readyToDeploy;
    outputStream << header;

    application::DeployPackToExternal deploy(from, to, statusLed);
    ExecuteAllActions();

    infra::ByteInputStream inputStream(to.sectors[0]);
    application::UpgradePackHeaderPrologue writtenHeader = {};
    inputStream >> infra::MakeByteRange(writtenHeader);

    EXPECT_TRUE(infra::ContentsEqual(infra::MakeByteRange(header), infra::MakeByteRange(writtenHeader)));
}

TEST_F(DeployPackToExternalTest, AfterCopyDeployStatusIsSet)
{
    infra::ByteOutputStream outputStream(from.sectors[0]);
    application::UpgradePackHeaderPrologue header = {};
    header.status = application::UpgradePackStatus::readyToDeploy;
    outputStream << header;

    application::DeployPackToExternal deploy(from, to, statusLed);
    ExecuteAllActions();

    infra::ByteInputStream inputStream(to.sectors[0]);
    application::UpgradePackHeaderPrologue writtenHeader = {};
    inputStream >> infra::MakeByteRange(writtenHeader);

    header.status = application::UpgradePackStatus::readyToDeploy;
    EXPECT_TRUE(infra::ContentsEqual(infra::MakeByteRange(header), infra::MakeByteRange(writtenHeader)));
}

TEST_F(DeployPackToExternalTest, DeployPackCopiesChunk)
{
    infra::ByteOutputStream outputStream(from.sectors[0]);
    application::UpgradePackHeaderPrologue header = {};
    header.status = application::UpgradePackStatus::readyToDeploy;
    std::array<uint8_t, 5> contents = { 1, 2, 3, 4, 5 };
    header.signedContentsLength = contents.size();
    outputStream << header << contents;

    application::DeployPackToExternal deploy(from, to, statusLed);
    ExecuteAllActions();

    infra::ByteInputStream inputStream(to.sectors[0]);
    application::UpgradePackHeaderPrologue writtenHeader = {};
    std::array<uint8_t, 5> writtenContents;
    inputStream >> infra::MakeByteRange(writtenHeader) >> writtenContents;

    EXPECT_EQ(contents, writtenContents);
}

TEST_F(DeployPackToExternalTest, DeployPackCopiesMultipleChunks)
{
    infra::ByteOutputStream outputStream(from.sectors[0]);
    application::UpgradePackHeaderPrologue header = {};
    header.status = application::UpgradePackStatus::readyToDeploy;
    std::array<uint8_t, 1024> largeContents;
    std::array<uint8_t, 5> contents = { 1, 2, 3, 4, 5 };
    header.signedContentsLength = contents.size() + largeContents.size();
    outputStream << header << largeContents << contents;

    application::DeployPackToExternal deploy(from, to, statusLed);
    ExecuteAllActions();

    infra::ByteInputStream inputStream(to.sectors[0]);
    application::UpgradePackHeaderPrologue writtenHeader = {};
    std::array<uint8_t, 5> writtenContents;
    inputStream >> infra::MakeByteRange(writtenHeader) >> largeContents >> writtenContents;

    EXPECT_EQ(contents, writtenContents);
}

TEST_F(DeployPackToExternalTest, AfterDeployDeployedStatusIsSet)
{
    infra::ByteOutputStream outputStream(from.sectors[0]);
    application::UpgradePackHeaderPrologue header = {};
    header.status = application::UpgradePackStatus::readyToDeploy;
    outputStream << header;

    application::DeployPackToExternal deploy(from, to, statusLed);
    ExecuteAllActions();

    infra::ByteInputStream inputStream(from.sectors[0]);
    inputStream >> header;
    EXPECT_EQ(application::UpgradePackStatus::deployed, header.status);
}

TEST_F(DeployPackToExternalTest, AfterDeployStatusLedSignalsSuccess)
{
    infra::ByteOutputStream outputStream(from.sectors[0]);
    application::UpgradePackHeaderPrologue header = {};
    header.status = application::UpgradePackStatus::readyToDeploy;
    outputStream << header;

    application::DeployPackToExternal deploy(from, to, statusLed);
    ExecuteAllActions();

    ForwardTime(std::chrono::milliseconds(1500));
    EXPECT_EQ((std::vector<hal::PinChange>{
        { std::chrono::milliseconds(0), true },
        { std::chrono::milliseconds(100), false },
        { std::chrono::milliseconds(200), true },
        { std::chrono::milliseconds(300), false }
    }), statusLed.PinChanges());
}

TEST_F(DeployPackToExternalTest, TooBigDoesNotDeploy)
{
    infra::ByteOutputStream outputStream(from.sectors[0]);
    application::UpgradePackHeaderPrologue header = {};
    header.status = application::UpgradePackStatus::readyToDeploy;
    header.signedContentsLength = to.SizeOfSector(0) * 5;
    outputStream << header;

    application::DeployPackToExternal deploy(from, to, statusLed);
    ExecuteAllActions();

    infra::ByteInputStream inputStream(to.sectors[0]);
    application::UpgradePackHeaderPrologue writtenHeader = {};
    inputStream >> infra::MakeByteRange(writtenHeader);

    EXPECT_FALSE(infra::ContentsEqual(infra::MakeByteRange(header), infra::MakeByteRange(writtenHeader)));
}

TEST_F(DeployPackToExternalTest, TooBigSignalsFailure)
{
    infra::ByteOutputStream outputStream(from.sectors[0]);
    application::UpgradePackHeaderPrologue header = {};
    header.status = application::UpgradePackStatus::readyToDeploy;
    header.signedContentsLength = to.SizeOfSector(0) * 5;
    outputStream << header;

    application::DeployPackToExternal deploy(from, to, statusLed);
    ExecuteAllActions();

    ForwardTime(std::chrono::milliseconds(400));
    EXPECT_EQ((std::vector<hal::PinChange>{
        { std::chrono::milliseconds(0), true},
        { std::chrono::milliseconds(100), false},
        { std::chrono::milliseconds(200), true},
        { std::chrono::milliseconds(300), false },
        { std::chrono::milliseconds(400), true},
    }), statusLed.PinChanges());
}
