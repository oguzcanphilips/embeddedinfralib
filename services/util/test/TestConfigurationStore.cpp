#include "gmock/gmock.h"
#include "hal/interfaces/test_doubles/FlashMock.hpp"
#include "infra/stream/ByteOutputStream.hpp"
#include "infra/util/test_helper/MockCallback.hpp"
#include "infra/util/test_helper/MockHelpers.hpp"
#include "services/util/ConfigurationStore.hpp"

class ConfigurationBlobTest
    : public testing::Test
{
public:
    ConfigurationBlobTest()
        : flash(1, 20)
        , configurationBlob(flash)
    {}

    MOCK_METHOD1(OnLoaded, void(bool success));

    void RecoverFromFlash()
    {
        EXPECT_CALL(flash, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone)));
        configurationBlob.Recover([this](bool success) { OnLoaded(success); });

        infra::ByteOutputStream stream(buffer);
        std::array<uint8_t, 8> hash = { 0x21, 0xcc, 0xca, 0x8b, 0xe7, 0x6b, 0x58, 0x7f };
        uint32_t size = 8;
        std::array<uint8_t, 8> data = { 0, 1, 2, 3, 4, 5, 6, 7 };

        stream << hash << size << data;

        EXPECT_CALL(*this, OnLoaded(true));
        onReadDone();
    }

public:
    testing::StrictMock<hal::CleanFlashMock> flash;

    infra::ByteRange buffer;
    infra::Function<void()> onReadDone;
    infra::Function<void()> onWriteDone;
    infra::Function<void()> onEraseDone;
    services::ConfigurationBlobImpl::WithStorage<8> configurationBlob;
};

TEST_F(ConfigurationBlobTest, fail_to_recover_from_empty_flash)
{
    EXPECT_CALL(flash, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone)));
    configurationBlob.Recover([this](bool success) { OnLoaded(success); });

    EXPECT_EQ(20, buffer.size());
    std::fill(buffer.begin(), buffer.end(), 0xff);

    EXPECT_CALL(*this, OnLoaded(false));
    onReadDone();
}

TEST_F(ConfigurationBlobTest, fail_to_recover_when_size_is_too_big)
{
    EXPECT_CALL(flash, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone)));
    configurationBlob.Recover([this](bool success) { OnLoaded(success); });

    infra::ByteOutputStream stream(buffer);
    std::array<uint8_t, 8> hash = {};
    uint32_t size = 9;
    stream << hash << size;

    EXPECT_CALL(*this, OnLoaded(false));
    onReadDone();
}

TEST_F(ConfigurationBlobTest, fail_to_recover_when_hash_is_incorrect)
{
    EXPECT_CALL(flash, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone)));
    configurationBlob.Recover([this](bool success) { OnLoaded(success); });

    infra::ByteOutputStream stream(buffer);
    std::array<uint8_t, 8> hash = {};
    uint32_t size = 8;
    std::array<uint8_t, 8> data = { 0, 1, 2, 3, 4, 5, 6, 7 };
    stream << hash << size << data;

    EXPECT_CALL(*this, OnLoaded(false));
    onReadDone();
}

TEST_F(ConfigurationBlobTest, recover_from_flash)
{
    EXPECT_CALL(flash, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone)));
    configurationBlob.Recover([this](bool success) { OnLoaded(success); });

    infra::ByteOutputStream stream(buffer);
    std::array<uint8_t, 8> hash = { 0x21, 0xcc, 0xca, 0x8b, 0xe7, 0x6b, 0x58, 0x7f };
    uint32_t size = 8;
    std::array<uint8_t, 8> data = { 0, 1, 2, 3, 4, 5, 6, 7 };

    stream << hash << size << data;

    EXPECT_CALL(*this, OnLoaded(true));
    onReadDone();

    EXPECT_EQ(data, configurationBlob.CurrentBlob());
}

TEST_F(ConfigurationBlobTest, Write_writes_to_flash)
{
    RecoverFromFlash();

    std::array<uint8_t, 8> data = { 1, 2, 3, 4, 5, 6, 7, 0 };
    infra::Copy(data, configurationBlob.MaxBlob());
    
    infra::ConstByteRange writeBuffer;
    EXPECT_CALL(flash, WriteBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&writeBuffer), testing::SaveArg<2>(&onWriteDone)));
    infra::VerifyingFunctionMock<void()> writeDone;
    configurationBlob.Write(8, writeDone);

    std::array<uint8_t, 20> blobData = { 0x21, 0xf1, 0xfb, 0x20, 0xaf, 0x91, 0x92, 0x86, 8, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 0 };
    EXPECT_EQ(blobData, writeBuffer);

    onWriteDone();
}

TEST_F(ConfigurationBlobTest, Erase_erases_flash)
{
    EXPECT_CALL(flash, EraseSectors(0, 1, testing::_)).WillOnce(testing::SaveArg<2>(&onEraseDone));
    infra::VerifyingFunctionMock<void()> eraseDone;
    configurationBlob.Erase(eraseDone);

    onEraseDone();
}

class ConfigurationBlobMock
    : public services::ConfigurationBlob
{
public:
    MOCK_METHOD0(CurrentBlob, infra::ByteRange());
    MOCK_METHOD0(MaxBlob, infra::ByteRange());
    MOCK_METHOD1(Recover, void(const infra::Function<void(bool success)>& onLoaded));
    MOCK_METHOD2(Write, void(uint32_t size, const infra::Function<void()>& onDone));
    MOCK_METHOD1(Erase, void(const infra::Function<void()>& onDone));
};

class ConfigurationStoreTest
    : public testing::Test
{
public:
    ConfigurationStoreTest()
        : execute([this]()
            {
                EXPECT_CALL(configurationBlob1, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
            })
        , configurationStore(configurationBlob1, configurationBlob2, [this](bool success) { OnLoaded(success); })
    {}

    MOCK_METHOD1(OnLoaded, void(bool success));

    struct Data
    {
        MOCK_METHOD1(Serialize, void(infra::ProtoFormatter& formatter));
        MOCK_METHOD1(Deserialize, void(infra::ProtoParser& parser));

    public:
        static const uint32_t maxMessageSize = 16;
    };

    void DontRecover()
    {
        EXPECT_CALL(configurationBlob2, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
        onRecoverDone(false);

        EXPECT_CALL(*this, OnLoaded(false));
        onRecoverDone(false);
    }

public:
    infra::Function<void(bool success)> onRecoverDone;
    testing::StrictMock<ConfigurationBlobMock> configurationBlob1;
    testing::StrictMock<ConfigurationBlobMock> configurationBlob2;
    infra::Execute execute;
    services::ConfigurationStore<testing::StrictMock<Data>> configurationStore;
};

TEST_F(ConfigurationStoreTest, failed_blob_load_is_propagated)
{
    EXPECT_CALL(configurationBlob2, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    onRecoverDone(false);

    EXPECT_CALL(*this, OnLoaded(false));
    onRecoverDone(false);
}

TEST_F(ConfigurationStoreTest, after_succesful_blob_load_configuration_is_available)
{
    EXPECT_CALL(configurationBlob1, CurrentBlob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(*this, OnLoaded(true));
    onRecoverDone(true);
}

TEST_F(ConfigurationStoreTest, after_succesful_blob_load_from_second_flash_configuration_is_available)
{
    EXPECT_CALL(configurationBlob2, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    onRecoverDone(false);

    EXPECT_CALL(configurationBlob2, CurrentBlob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(*this, OnLoaded(true));
    onRecoverDone(true);
}

TEST_F(ConfigurationStoreTest, Write_writes_to_blob)
{
    DontRecover();

    infra::Function<void()> onWriteDone;
    std::array<uint8_t, 32> data;
    EXPECT_CALL(configurationBlob1, MaxBlob()).WillOnce(testing::Return(infra::MakeRange(data)));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_)).WillOnce(infra::Lambda([](infra::ProtoFormatter& formatter) { formatter.PutFixed32(1); }));
    EXPECT_CALL(configurationBlob1, Write(4, testing::_)).WillOnce(testing::SaveArg<1>(&onWriteDone));
    configurationStore.Write();

    infra::Function<void()> onEraseDone;
    EXPECT_CALL(configurationBlob2, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onWriteDone();

    onEraseDone();
}

TEST_F(ConfigurationStoreTest, Write_writes_to_other_blob_than_recovered)
{
    EXPECT_CALL(configurationBlob1, CurrentBlob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(*this, OnLoaded(true));
    onRecoverDone(true);

    infra::Function<void()> onWriteDone;
    std::array<uint8_t, 32> data;
    EXPECT_CALL(configurationBlob2, MaxBlob()).WillOnce(testing::Return(infra::MakeRange(data)));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_)).WillOnce(infra::Lambda([](infra::ProtoFormatter& formatter) { formatter.PutFixed32(1); }));
    EXPECT_CALL(configurationBlob2, Write(4, testing::_)).WillOnce(testing::SaveArg<1>(&onWriteDone));
    configurationStore.Write();

    infra::Function<void()> onEraseDone;
    EXPECT_CALL(configurationBlob1, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onWriteDone();

    onEraseDone();
}

TEST_F(ConfigurationStoreTest, double_Write_is_held)
{
    DontRecover();

    infra::Function<void()> onWriteDone;
    std::array<uint8_t, 32> data;
    EXPECT_CALL(configurationBlob1, MaxBlob()).WillOnce(testing::Return(infra::MakeRange(data)));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_)).WillOnce(infra::Lambda([](infra::ProtoFormatter& formatter) { formatter.PutFixed32(1); }));
    EXPECT_CALL(configurationBlob1, Write(4, testing::_)).WillOnce(testing::SaveArg<1>(&onWriteDone));
    configurationStore.Write();
    configurationStore.Write();

    infra::Function<void()> onEraseDone;
    EXPECT_CALL(configurationBlob2, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onWriteDone();

    EXPECT_CALL(configurationBlob2, MaxBlob()).WillOnce(testing::Return(infra::MakeRange(data)));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_)).WillOnce(infra::Lambda([](infra::ProtoFormatter& formatter) { formatter.PutFixed32(1); }));
    EXPECT_CALL(configurationBlob2, Write(4, testing::_)).WillOnce(testing::SaveArg<1>(&onWriteDone));
    onEraseDone();

    EXPECT_CALL(configurationBlob1, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onWriteDone();
    onEraseDone();
}

TEST_F(ConfigurationStoreTest, during_Lock_Write_is_held)
{
    DontRecover();

    infra::Optional<services::ConfigurationStoreBase::LockGuard> lock(infra::inPlace, configurationStore.Lock());
    configurationStore.Write();

    infra::Function<void()> onWriteDone;
    std::array<uint8_t, 32> data;
    EXPECT_CALL(configurationBlob1, MaxBlob()).WillOnce(testing::Return(infra::MakeRange(data)));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_)).WillOnce(infra::Lambda([](infra::ProtoFormatter& formatter) { formatter.PutFixed32(1); }));
    EXPECT_CALL(configurationBlob1, Write(4, testing::_)).WillOnce(testing::SaveArg<1>(&onWriteDone));
    lock = infra::none;
}

TEST_F(ConfigurationStoreTest, onDone_is_called_when_done)
{
    DontRecover();

    infra::Function<void()> onWriteDone;
    std::array<uint8_t, 32> data;
    EXPECT_CALL(configurationBlob1, MaxBlob()).WillOnce(testing::Return(infra::MakeRange(data)));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_)).WillOnce(infra::Lambda([](infra::ProtoFormatter& formatter) { formatter.PutFixed32(1); }));
    EXPECT_CALL(configurationBlob1, Write(4, testing::_)).WillOnce(testing::SaveArg<1>(&onWriteDone));

    infra::VerifyingFunctionMock<void()> onDone;
    configurationStore.Write(onDone);

    infra::Function<void()> onEraseDone;
    EXPECT_CALL(configurationBlob2, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onWriteDone();

    onEraseDone();
}
