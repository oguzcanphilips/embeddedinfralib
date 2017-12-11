#include "gmock/gmock.h"
#include "hal/synchronous_interfaces/test_doubles/SynchronousFlashStub.hpp"
#include "upgrade/boot_loader/DecryptorNone.hpp"
#include "upgrade/boot_loader/ImageUpgraderDiComm.hpp"
#include "upgrade/boot_loader/test/MockDecryptor.hpp"
#include "upgrade/boot_loader/test/MockDiComm.hpp"
#include "upgrade/pack/UpgradePackHeader.hpp"

class TimeKeeperDicomm
    : public hal::TimeKeeper
{
public:
    virtual bool Timeout() override
    {
        return true;
    }

    virtual void Reset() override
    {}
};

class ImageUpgraderDiCommTest
    : public testing::Test
{
public:
    ImageUpgraderDiCommTest()
        : upgrader("product", decryptor, diComm, timeKeeperStub, timeKeeperStub)
        , upgradePackFlash(1, 1024)
    {}

    application::DecryptorNone decryptor;
    testing::StrictMock<application::MockDiComm> diComm;
    TimeKeeperDicomm timeKeeperStub;
    application::ImageUpgraderDiComm upgrader;
    hal::SynchronousFlashStub upgradePackFlash;
    testing::InSequence s;
};

struct Ok {};

TEST_F(ImageUpgraderDiCommTest, ConstructionWithTargetName)
{
    EXPECT_STREQ("product", upgrader.TargetName());
}

TEST_F(ImageUpgraderDiCommTest, UpgradeInitializesCommunication)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Throw<Ok>(Ok()));

    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 0, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, UpgradeSetsStateToIdle)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Throw<Ok>(Ok()));

    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 0, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, UpgradeFailsWhenSettingStateToIdleFails)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(false, "")));

    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 0, 0));
}

TEST_F(ImageUpgraderDiCommTest, UpgradeChecksStateIsIdle)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", testing::_)).WillOnce(testing::Throw<Ok>(Ok()));

    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 0, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, UpgradeFailsWhenGetPropsFails)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));

    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 0, 0));
}

TEST_F(ImageUpgraderDiCommTest, UpgradeFailsWhenStateIsNotIdle)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"ready"})")));

    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 0, 0));
}

TEST_F(ImageUpgraderDiCommTest, UpgradeFailsWhenCanUpgradeIsFalse)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle","canupgrade":false})")));

    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 0, 0));
}

TEST_F(ImageUpgraderDiCommTest, UpgradeFailsWhenCanUpgradeIsInvalid)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle","canupgrade":invalid})")));

    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 0, 0));
}

TEST_F(ImageUpgraderDiCommTest, UpgradeSetsMandatoryUpgradeAndSize)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":10})")).WillOnce(testing::Throw<Ok>(Ok()));

    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 10, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, UpgradeSetsDifferentSize)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":20})")).WillOnce(testing::Throw<Ok>(Ok()));

    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 20, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, UpgradeFailsWhenPutPropsFails)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":10})")).WillOnce(testing::Return(std::make_pair(false, "")));

    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 10, 0));
}

TEST_F(ImageUpgraderDiCommTest, OneChunkIsSent)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Throw<Ok>(Ok()));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 4, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, UpgradeFailsWhenChunkFails)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(false, "")));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 4, 0));
}

TEST_F(ImageUpgraderDiCommTest, LargeRequestedChunkIsLimited)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle","maxchunksize":2048})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":2048})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZGFiY2RhYmNkYWJjZA=="})")).WillOnce(testing::Throw<Ok>(Ok()));
    
    upgradePackFlash.sectors[0] = { 'a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d',
        'a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d',
        'a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d', 
        'a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d',
        'a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d',
        'a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d','a','b','c','d', };

    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 2048, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, ASmallChunkIsSentWhenSizeIsRestricted)
{
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle","maxchunksize":3})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":8})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJj"})")).WillOnce(testing::Throw<Ok>(Ok()));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 8, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, UpgradeFailsWhenErrorIsReturnedInsteadOfProgress)
{
    testing::InSequence s;
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"error"})")));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 4, 0));
}

TEST_F(ImageUpgraderDiCommTest, NextChunkIsSentAfterPreviousHasBeenProcessed)
{
    testing::InSequence s;
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle","maxchunksize":3})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":6})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJj"})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":3})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"ZGVm"})")).WillOnce(testing::Throw<Ok>(Ok()));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 6, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, StateIsPolledAfterDownloadComplete)
{
    testing::InSequence s;

    //InitializeProtocol
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    //PrepareDownload
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));

    //SendFirmware
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":4})")));

    //WaitForProgrammingCompletion
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));

    //InitializeProtocol
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //WaitForState
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Throw<Ok>(Ok()));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 4, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTest, UpgradeFailsWhenPollForIdleFails)
{
    testing::InSequence s;

    //InitializeProtocol
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    //PrepareDownload
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));

    //SendFirmware
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":4})")));

    //WaitForProgrammingCompletion
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));
    
    //InitializeProtocol
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //WaitForState
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 4, 0));
}

TEST_F(ImageUpgraderDiCommTest, WhenStateIsErrorUpgradeIsNotSuccessful)
{
    testing::InSequence s;

    //InitializeProtocol
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    //PrepareDownload
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));

    //SendFirmware
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":4})")));

    //WaitForProgrammingCompletion
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));

    //InitializeProtocol
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //WaitForState
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"error"})")));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 4, 0));
}

TEST_F(ImageUpgraderDiCommTest, AfterStateIsIdleUpgradeIsSuccessful)
{
    testing::InSequence s;

    //InitializeProtocol
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));

    //PrepareDownload
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));

    //SendFirmware
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":4})")));

    //WaitForProgrammingCompletion
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));

    //InitializeProtocol
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //WaitForState
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_EQ(0, upgrader.Upgrade(upgradePackFlash, 0, 4, 0));
}

class ImageUpgraderDiCommDecryptionTest
    : public testing::Test
{
public:
    ImageUpgraderDiCommDecryptionTest()
        : upgrader("product", decryptor, diComm, timeKeeperStub, timeKeeperStub)
        , upgradePackFlash(1, 512)
    {}

    testing::StrictMock<application::MockDecryptor> decryptor;
    testing::StrictMock<application::MockDiComm> diComm;
    TimeKeeperDicomm timeKeeperStub;
    application::ImageUpgraderDiComm upgrader;
    hal::SynchronousFlashStub upgradePackFlash;
};

TEST_F(ImageUpgraderDiCommDecryptionTest, FirmwareIsDecrypted)
{
    testing::InSequence s;
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));
    EXPECT_CALL(decryptor, DecryptPartMock(std::vector<uint8_t>{ 'a', 'b', 'c', 'd' })).WillOnce(testing::Return(std::vector<uint8_t>{ 'e', 'f', 'g', 'h' }));
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"ZWZnaA=="})")).WillOnce(testing::Throw<Ok>(Ok()));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 4, 0), Ok);
}

class TimeKeeperMock
    : public hal::TimeKeeper
{
public:
    virtual bool Timeout() override
    {
        return TimeoutMock();
    }

    virtual void Reset() override
    {
        ResetMock();
    }

    MOCK_METHOD0(TimeoutMock, bool());
    MOCK_METHOD0(ResetMock, void());
};

class ImageUpgraderDiCommTimeoutTest
    : public testing::Test
{
public:
    ImageUpgraderDiCommTimeoutTest()
        : upgrader("product", decryptor, diComm, timeKeeperTimeout, timeKeeperPollDelay)
        , upgradePackFlash(1, 1024)
    {}

    application::DecryptorNone decryptor;
    testing::StrictMock<application::MockDiComm> diComm;
    testing::StrictMock<TimeKeeperMock> timeKeeperTimeout;
    testing::StrictMock<TimeKeeperMock> timeKeeperPollDelay;
    application::ImageUpgraderDiComm upgrader;
    hal::SynchronousFlashStub upgradePackFlash;
    testing::InSequence s;
};

TEST_F(ImageUpgraderDiCommTimeoutTest, InitializationIsRetriedAfterFailure)
{
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Throw<Ok>(Ok()));

    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 0, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTimeoutTest, UpgradeFailsWhenInitializationFailsAfterTimeout)
{
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));
    
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));

    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(true));

    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 0, 0));
}

TEST_F(ImageUpgraderDiCommTimeoutTest, UpgradeContinuesWhenInitializationSucceedsAfterRetrying)
{
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));

    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Throw<Ok>(Ok()));
    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 0, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTimeoutTest, UpgradeWaitsForStateDownloadingWhenStateIsPreparing)
{
    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    //PrepareDownload
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));

    //WaitForState
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"preparing"})")));

    EXPECT_CALL(timeKeeperPollDelay, ResetMock());
    EXPECT_CALL(timeKeeperPollDelay, TimeoutMock()).WillOnce(testing::Return(true));

    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false)); 
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));
    EXPECT_CALL(diComm, PutPropsMock("firmware", testing::_)).WillOnce(testing::Throw<Ok>(Ok()));

    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 4, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTimeoutTest, WaitForProgrammingCompletionSuccess)
{
    testing::InSequence s;
    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    //PrepareDownload
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));

    //SendFirmware
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":4})")));

    //WaitForProgrammingCompletion
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"checking"})")));
    EXPECT_CALL(timeKeeperPollDelay, ResetMock());
    EXPECT_CALL(timeKeeperPollDelay, TimeoutMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));

    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"ready"})")));
    EXPECT_CALL(timeKeeperPollDelay, ResetMock());
    EXPECT_CALL(timeKeeperPollDelay, TimeoutMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));

    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"programming"})")));
    EXPECT_CALL(timeKeeperPollDelay, ResetMock());
    EXPECT_CALL(timeKeeperPollDelay, TimeoutMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));

    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));

    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Throw<Ok>(Ok()));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 4, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTimeoutTest, WaitForProgrammingCompletionTimeout)
{
    testing::InSequence s;
    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    //PrepareDownload
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));

    //SendFirmware
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":4})")));

    //WaitForProgrammingCompletion
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"checking"})")));
    EXPECT_CALL(timeKeeperPollDelay, ResetMock());
    EXPECT_CALL(timeKeeperPollDelay, TimeoutMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(true));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 4, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTimeoutTest, WhenStateIsNotIdleStateIsPolledAgain)
{
    testing::InSequence s;
    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    //PrepareDownload
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));

    //SendFirmware
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":4})")));

    //WaitForProgrammingCompletion
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));

    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //WaitForState
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"programming"})")));
    EXPECT_CALL(timeKeeperPollDelay, ResetMock());
    EXPECT_CALL(timeKeeperPollDelay, TimeoutMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Throw<Ok>(Ok()));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_THROW(upgrader.Upgrade(upgradePackFlash, 0, 4, 0), Ok);
}

TEST_F(ImageUpgraderDiCommTimeoutTest, SuccesssfulWaitingForIdleStateWithRetriesgDueToTimeout)
{
    testing::InSequence s;
    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    //PrepareDownload
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));

    //SendFirmware
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":4})")));
    
    //WaitForProgrammingCompletion
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));

    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //WaitForState
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"programming"})")));
    EXPECT_CALL(timeKeeperPollDelay, ResetMock());
    EXPECT_CALL(timeKeeperPollDelay, TimeoutMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    
    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_EQ(0, upgrader.Upgrade(upgradePackFlash, 0, 4, 0));
}

TEST_F(ImageUpgraderDiCommTimeoutTest, NotSuccesssfulWaitingForIdleStateWithRetriesgDueToTimeout)
{
    testing::InSequence s;
    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //InitializeProperties
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"state":"idle"})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"idle"})")));

    //PrepareDownload
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"mandatory":true,"state":"downloading","size":4})")).WillOnce(testing::Return(std::make_pair(true, "")));
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading"})")));

    //SendFirmware
    EXPECT_CALL(diComm, PutPropsMock("firmware", R"({"data":"YWJjZA=="})")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"downloading","progress":4})")));

    //WaitForProgrammingCompletion
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));

    //InitializeProtocol
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, InitializeMock()).WillOnce(testing::Return(true));

    //WaitForState
    EXPECT_CALL(timeKeeperTimeout, ResetMock());
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(true, R"({"state":"programming"})")));
    EXPECT_CALL(timeKeeperPollDelay, ResetMock());
    EXPECT_CALL(timeKeeperPollDelay, TimeoutMock()).WillOnce(testing::Return(true));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(false));
    EXPECT_CALL(diComm, GetPropsMock("firmware")).WillOnce(testing::Return(std::make_pair(false, "")));
    EXPECT_CALL(timeKeeperTimeout, TimeoutMock()).WillOnce(testing::Return(true));

    upgradePackFlash.sectors[0] = { 'a', 'b', 'c', 'd' };
    EXPECT_EQ(application::upgradeErrorCodeExternalImageUpgradeFailed, upgrader.Upgrade(upgradePackFlash, 0, 4, 0));
}