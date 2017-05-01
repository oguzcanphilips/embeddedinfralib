#include "gmock/gmock.h"
#include "infra/stream/public/ByteOutputStream.hpp"
#include "hal/synchronous_interfaces/test_doubles/public/SynchronousFlashStub.hpp"
#include "packs/upgrade/boot_loader/public/DecryptorNone.hpp"
#include "packs/upgrade/pack/public/UpgradePackHeader.hpp"
#include "packs/upgrade/boot_loader/public/PackUpgrader.hpp"

class ImageUpgraderMock
    : public application::ImageUpgrader
{
public:
    ImageUpgraderMock()
        : application::ImageUpgrader("upgrader", decryptorNone)
    {}

    virtual uint32_t Upgrade(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t destinationAddress) override
    {
        return UpgradeMock(imageAddress, imageSize, destinationAddress);
    }

    MOCK_METHOD3(UpgradeMock, uint32_t(uint32_t, uint32_t, uint32_t));

    application::DecryptorNone decryptorNone;
};

class PackUpgraderTest
    : public testing::Test
{
public:
    PackUpgraderTest()
        : upgradePackFlash(1, 4096)
        , singleUpgraderMock({ &imageUpgraderMock })
    {}

    void AssignZeroFilledString(const std::string& from, infra::MemoryRange<char> to) const
    {
        std::copy(from.begin(), from.begin() + std::min(from.size(), to.size()), to.begin());
        std::fill(to.begin() + std::min(from.size(), to.size()), to.end(), 0);
    }

    struct UpgradePackHeaderNoSecurity
    {
        application::UpgradePackHeaderPrologue prologue;
        application::UpgradePackHeaderEpilogue epilogue;
    };

    UpgradePackHeaderNoSecurity CreateReadyToDeployHeader(std::size_t numberOfImages) const
    {
        UpgradePackHeaderNoSecurity header = {};
        header.prologue.status = application::UpgradePackStatus::readyToDeploy;
        header.prologue.magic = application::upgradePackMagic;
        header.prologue.errorCode = 0xffffffff;
        header.prologue.signedContentsLength = sizeof(application::UpgradePackHeaderEpilogue);
        header.epilogue.headerVersion = 1;
        header.epilogue.numberOfImages = numberOfImages;

        return header;
    }

    application::ImageHeaderPrologue CreateImageHeaderPrologue(const std::string& targetName, std::size_t ExtraSize) const
    {
        application::ImageHeaderPrologue header;
        header.lengthOfHeaderAndImage = sizeof(header) + ExtraSize;
        AssignZeroFilledString(targetName, header.targetName);
        header.encryptionAndMacMethod = 0;

        return header;
    }

    application::ImageHeaderEpilogue CreateImageHeaderEpilogue() const
    {
        application::ImageHeaderEpilogue header{ 1, 2 };

        return header;
    }

public:
    hal::SynchronousFlashStub upgradePackFlash;
    testing::StrictMock<ImageUpgraderMock> imageUpgraderMock;
    std::array<application::ImageUpgrader*, 1> singleUpgraderMock;
};

TEST_F(PackUpgraderTest, Construction)
{
    application::PackUpgrader upgrader(upgradePackFlash);
}

TEST_F(PackUpgraderTest, ImageInvokesImageUpgrader)
{
    UpgradePackHeaderNoSecurity header(CreateReadyToDeployHeader(1));

    const std::vector<uint8_t> image{ 1, 5 };
    application::ImageHeaderPrologue imageHeaderPrologue(CreateImageHeaderPrologue("upgrader", image.size()));
    application::ImageHeaderEpilogue imageHeaderEpilogue = CreateImageHeaderEpilogue();

    infra::ByteOutputStream stream(upgradePackFlash.sectors[0]);
    stream << header << imageHeaderPrologue << imageHeaderEpilogue << infra::ConstByteRange(image);

    EXPECT_CALL(imageUpgraderMock, UpgradeMock(244, 2, 1)).WillOnce(testing::Return(0));
    application::PackUpgrader packUpgrader(upgradePackFlash);
    packUpgrader.UpgradeFromImages(singleUpgraderMock);
}

TEST_F(PackUpgraderTest, PackIsMarkedAsDeployed)
{
    UpgradePackHeaderNoSecurity header(CreateReadyToDeployHeader(1));

    const std::vector<uint8_t> image{ 1, 5 };
    application::ImageHeaderPrologue imageHeaderPrologue(CreateImageHeaderPrologue("upgrader", image.size()));
    application::ImageHeaderEpilogue imageHeaderEpilogue = CreateImageHeaderEpilogue();

    infra::ByteOutputStream stream(upgradePackFlash.sectors[0]);
    stream << header << imageHeaderPrologue << imageHeaderEpilogue << infra::ConstByteRange(image);

    EXPECT_CALL(imageUpgraderMock, UpgradeMock(244, 2, 1)).WillOnce(testing::Return(0));
    application::PackUpgrader packUpgrader(upgradePackFlash);
    packUpgrader.UpgradeFromImages(singleUpgraderMock);

    EXPECT_EQ(application::UpgradePackStatus::deployed, static_cast<application::UpgradePackStatus>(upgradePackFlash.sectors[0][0]));
}

TEST_F(PackUpgraderTest, WhenVersionIsIncorrectPackIsMarkedAsError)
{
    UpgradePackHeaderNoSecurity header(CreateReadyToDeployHeader(1));
    header.epilogue.headerVersion = 2;

    const std::vector<uint8_t> image{ 1, 5 };
    application::ImageHeaderPrologue imageHeaderPrologue(CreateImageHeaderPrologue("upgrader", image.size()));
    application::ImageHeaderEpilogue imageHeaderEpilogue = CreateImageHeaderEpilogue();

    infra::ByteOutputStream stream(upgradePackFlash.sectors[0]);
    stream << header << imageHeaderPrologue << imageHeaderEpilogue << infra::ConstByteRange(image);

    application::PackUpgrader packUpgrader(upgradePackFlash);
    packUpgrader.UpgradeFromImages(singleUpgraderMock);

    EXPECT_FALSE(upgradePackFlash.sectors[0][0] & ~static_cast<uint8_t>(application::UpgradePackStatus::invalid));

    application::UpgradePackHeaderPrologue& prologue = reinterpret_cast<application::UpgradePackHeaderPrologue&>(upgradePackFlash.sectors[0][0]);
    EXPECT_FALSE(static_cast<uint8_t>(prologue.status) & ~static_cast<uint8_t>(application::UpgradePackStatus::invalid));
    EXPECT_EQ(application::upgradeErrorCodeUnknownHeaderVersion, prologue.errorCode);
}

TEST_F(PackUpgraderTest, WhenUpgraderIsNotFoundPackIsMarkedAsError)
{
    UpgradePackHeaderNoSecurity header(CreateReadyToDeployHeader(1));

    const std::vector<uint8_t> image{ 1, 5 };
    application::ImageHeaderPrologue imageHeaderPrologue(CreateImageHeaderPrologue("unknown", image.size()));
    application::ImageHeaderEpilogue imageHeaderEpilogue = CreateImageHeaderEpilogue();

    infra::ByteOutputStream stream(upgradePackFlash.sectors[0]);
    stream << header << imageHeaderPrologue << imageHeaderEpilogue << infra::ConstByteRange(image);

    application::PackUpgrader packUpgrader(upgradePackFlash);
    packUpgrader.UpgradeFromImages(singleUpgraderMock);

    EXPECT_FALSE(upgradePackFlash.sectors[0][0] & ~static_cast<uint8_t>(application::UpgradePackStatus::invalid));

    application::UpgradePackHeaderPrologue& prologue = reinterpret_cast<application::UpgradePackHeaderPrologue&>(upgradePackFlash.sectors[0][0]);
    EXPECT_FALSE(static_cast<uint8_t>(prologue.status) & ~static_cast<uint8_t>(application::UpgradePackStatus::invalid));
    EXPECT_EQ(application::upgradeErrorCodeNoSuitableImageUpgraderFound, prologue.errorCode);
}

TEST_F(PackUpgraderTest, WhenUpgraderCannotUpgradePackIsMarkedAsError)
{
    UpgradePackHeaderNoSecurity header(CreateReadyToDeployHeader(1));

    const std::vector<uint8_t> image{ 1, 5 };
    application::ImageHeaderPrologue imageHeaderPrologue(CreateImageHeaderPrologue("upgrader", image.size()));
    application::ImageHeaderEpilogue imageHeaderEpilogue = CreateImageHeaderEpilogue();

    infra::ByteOutputStream stream(upgradePackFlash.sectors[0]);
    stream << header << imageHeaderPrologue << imageHeaderEpilogue << infra::ConstByteRange(image);

    EXPECT_CALL(imageUpgraderMock, UpgradeMock(244, 2, 1)).WillOnce(testing::Return(application::upgradeErrorCodeImageUpgradeFailed));
    application::PackUpgrader packUpgrader(upgradePackFlash);
    packUpgrader.UpgradeFromImages(singleUpgraderMock);

    EXPECT_FALSE(upgradePackFlash.sectors[0][0] & ~static_cast<uint8_t>(application::UpgradePackStatus::invalid));

    application::UpgradePackHeaderPrologue& prologue = reinterpret_cast<application::UpgradePackHeaderPrologue&>(upgradePackFlash.sectors[0][0]);
    EXPECT_FALSE(static_cast<uint8_t>(prologue.status) & ~static_cast<uint8_t>(application::UpgradePackStatus::invalid));
    EXPECT_EQ(application::upgradeErrorCodeImageUpgradeFailed, prologue.errorCode);
}
