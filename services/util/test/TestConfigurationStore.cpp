#include "gmock/gmock.h"
#include "hal/interfaces/test_doubles/FlashMock.hpp"
#include "infra/stream/ByteOutputStream.hpp"
#include "infra/util/test_helper/MockCallback.hpp"
#include "services/util/ConfigurationStore.hpp"

class ConfigurationBlobTest
    : public testing::Test
{
public:
    ConfigurationBlobTest()
        : flash1(1, 16)
        , flash2(1, 16)
        , execute([this]() { EXPECT_CALL(flash1, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone))); })
        , configurationBlob(flash1, flash2, [this](bool success) { OnLoaded(success); })
    {}

    MOCK_METHOD1(OnLoaded, void(bool success));

    void RecoverFromFlash1()
    {
        infra::ByteOutputStream stream(buffer);
        std::array<uint8_t, 8> hash = { 0x21, 0xcc, 0xca, 0x8b, 0xe7, 0x6b, 0x58, 0x7f };
        uint32_t size = 8;
        std::array<uint8_t, 8> data = { 0, 1, 2, 3, 4, 5, 6, 7 };

        stream << hash << size << data;

        EXPECT_CALL(flash2, EraseSectors(0, 1, testing::_)).WillOnce(testing::SaveArg<2>(&onEraseDone));
        onReadDone();

        EXPECT_CALL(*this, OnLoaded(true));
        onEraseDone();
    }

public:
    testing::StrictMock<hal::CleanFlashMock> flash1;
    testing::StrictMock<hal::CleanFlashMock> flash2;

    infra::ByteRange buffer;
    infra::Function<void()> onReadDone;
    infra::Function<void()> onWriteDone;
    infra::Function<void()> onEraseDone;
    infra::Execute execute;
    services::ConfigurationBlobImpl::WithStorage<8> configurationBlob;
};

TEST_F(ConfigurationBlobTest, fail_to_recover_from_empty_flash)
{
    EXPECT_EQ(20, buffer.size());
    std::fill(buffer.begin(), buffer.end(), 0xff);

    EXPECT_CALL(flash2, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone)));
    onReadDone();

    std::fill(buffer.begin(), buffer.end(), 0xff);
    EXPECT_CALL(*this, OnLoaded(false));
    onReadDone();
}

TEST_F(ConfigurationBlobTest, fail_to_recover_when_size_is_too_big)
{
    infra::ByteOutputStream stream(buffer);
    std::array<uint8_t, 8> hash = {};
    uint32_t size = 9;
    stream << hash << size;

    EXPECT_CALL(flash2, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone)));
    onReadDone();
}

TEST_F(ConfigurationBlobTest, fail_to_recover_when_hash_is_incorrect)
{
    infra::ByteOutputStream stream(buffer);
    std::array<uint8_t, 8> hash = {};
    uint32_t size = 8;
    std::array<uint8_t, 8> data = { 0, 1, 2, 3, 4, 5, 6, 7 };
    stream << hash << size << data;

    EXPECT_CALL(flash2, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone)));
    onReadDone();
}

TEST_F(ConfigurationBlobTest, recover_from_flash1)
{
    infra::ByteOutputStream stream(buffer);
    std::array<uint8_t, 8> hash = { 0x21, 0xcc, 0xca, 0x8b, 0xe7, 0x6b, 0x58, 0x7f };
    uint32_t size = 8;
    std::array<uint8_t, 8> data = { 0, 1, 2, 3, 4, 5, 6, 7 };

    stream << hash << size << data;

    EXPECT_CALL(flash2, EraseSectors(0, 1, testing::_)).WillOnce(testing::SaveArg<2>(&onEraseDone));
    onReadDone();

    EXPECT_CALL(*this, OnLoaded(true));
    onEraseDone();

    EXPECT_EQ(data, configurationBlob.Blob());
}

TEST_F(ConfigurationBlobTest, recover_from_flash2)
{
    std::fill(buffer.begin(), buffer.end(), 0xff);
    EXPECT_CALL(flash2, ReadBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&buffer), testing::SaveArg<2>(&onReadDone)));
    onReadDone();

    infra::ByteOutputStream stream(buffer);
    std::array<uint8_t, 8> hash = { 0x21, 0xcc, 0xca, 0x8b, 0xe7, 0x6b, 0x58, 0x7f };
    uint32_t size = 8;
    std::array<uint8_t, 8> data = { 0, 1, 2, 3, 4, 5, 6, 7 };

    stream << hash << size << data;

    EXPECT_CALL(flash1, EraseSectors(0, 1, testing::_)).WillOnce(testing::SaveArg<2>(&onEraseDone));
    onReadDone();

    EXPECT_CALL(*this, OnLoaded(true));
    onEraseDone();

    EXPECT_EQ(data, configurationBlob.Blob());
}

TEST_F(ConfigurationBlobTest, Write_writes_to_inactive_flash)
{
    RecoverFromFlash1();

    std::array<uint8_t, 8> data = { 1, 2, 3, 4, 5, 6, 7, 0 };
    infra::Copy(data, configurationBlob.Blob());
    
    infra::ConstByteRange writeBuffer;
    EXPECT_CALL(flash2, WriteBuffer(testing::_, 0, testing::_)).WillOnce(testing::DoAll(testing::SaveArg<0>(&writeBuffer), testing::SaveArg<2>(&onWriteDone)));
    infra::VerifyingFunctionMock<void()> writeDone;
    configurationBlob.Write(writeDone);

    std::array<uint8_t, 20> blobData = { 0x21, 0xf1, 0xfb, 0x20, 0xaf, 0x91, 0x92, 0x86, 8, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 0 };
    EXPECT_EQ(blobData, writeBuffer);
    onWriteDone();
}

class ConfigurationBlobMock
    : public services::ConfigurationBlob
{
public:
    MOCK_METHOD0(Blob, infra::ByteRange());
    MOCK_METHOD1(Write, void(const infra::Function<void()>& onDone));
};

class ConfigurationStoreTest
    : public testing::Test
{
public:
    ConfigurationStoreTest()
        : configurationStore(configurationBlob, [this](bool success) { OnLoaded(success); })
    {}

    MOCK_METHOD1(OnLoaded, void(bool success));

    struct Data
    {
        MOCK_METHOD1(Serialize, void(infra::ProtoFormatter& formatter));
        MOCK_METHOD1(Deserialize, void(infra::ProtoParser& parser));

    public:
        static const uint32_t maxMessageSize = 16;
    };

public:
    ConfigurationBlobMock configurationBlob;
    services::ConfigurationStore<testing::StrictMock<Data>> configurationStore;
};

TEST_F(ConfigurationStoreTest, failed_blob_load_is_propagated)
{
    EXPECT_CALL(*this, OnLoaded(false));
    configurationStore.OnBlobLoaded(false);
}

TEST_F(ConfigurationStoreTest, after_succesful_blob_load_configuration_is_available)
{
    EXPECT_CALL(configurationBlob, Blob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(*this, OnLoaded(true));
    configurationStore.OnBlobLoaded(true);
}

TEST_F(ConfigurationStoreTest, Write_writes_to_blob)
{
    EXPECT_CALL(*this, OnLoaded(false));
    configurationStore.OnBlobLoaded(false);

    infra::Function<void()> onWriteDone;
    EXPECT_CALL(configurationBlob, Blob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_));
    EXPECT_CALL(configurationBlob, Write(testing::_)).WillOnce(testing::SaveArg<0>(&onWriteDone));
    configurationStore.Write();
}

TEST_F(ConfigurationStoreTest, double_Write_is_held)
{
    EXPECT_CALL(*this, OnLoaded(false));
    configurationStore.OnBlobLoaded(false);

    infra::Function<void()> onWriteDone;
    EXPECT_CALL(configurationBlob, Blob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_));
    EXPECT_CALL(configurationBlob, Write(testing::_)).WillOnce(testing::SaveArg<0>(&onWriteDone));
    configurationStore.Write();
    configurationStore.Write();

    EXPECT_CALL(configurationBlob, Blob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_));
    EXPECT_CALL(configurationBlob, Write(testing::_)).WillOnce(testing::SaveArg<0>(&onWriteDone));
    onWriteDone();
}

TEST_F(ConfigurationStoreTest, during_Lock_Write_is_held)
{
    EXPECT_CALL(*this, OnLoaded(false));
    configurationStore.OnBlobLoaded(false);

    infra::Optional<services::ConfigurationStoreBase::LockGuard> lock(infra::inPlace, configurationStore.Lock());
    configurationStore.Write();

    infra::Function<void()> onWriteDone;
    EXPECT_CALL(configurationBlob, Blob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_));
    EXPECT_CALL(configurationBlob, Write(testing::_)).WillOnce(testing::SaveArg<0>(&onWriteDone));
    lock = infra::none;
}

TEST_F(ConfigurationStoreTest, onDone_is_called_when_done)
{
    EXPECT_CALL(*this, OnLoaded(false));
    configurationStore.OnBlobLoaded(false);

    infra::Function<void()> onWriteDone;
    EXPECT_CALL(configurationBlob, Blob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_));
    EXPECT_CALL(configurationBlob, Write(testing::_)).WillOnce(testing::SaveArg<0>(&onWriteDone));

    infra::VerifyingFunctionMock<void()> onDone;
    configurationStore.Write(onDone);
    onWriteDone();
}
