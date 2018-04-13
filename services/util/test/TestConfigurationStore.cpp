#include "gmock/gmock.h"
#include "hal/interfaces/test_doubles/FlashMock.hpp"
#include "hal/interfaces/test_doubles/FlashStub.hpp"
#include "infra/event/test_helper/EventDispatcherFixture.hpp"
#include "infra/stream/ByteOutputStream.hpp"
#include "infra/util/test_helper/MockCallback.hpp"
#include "infra/util/test_helper/MockHelpers.hpp"
#include "services/util/ConfigurationStore.hpp"

namespace
{
    struct Data
    {
        MOCK_METHOD1(Serialize, void(infra::ProtoFormatter& formatter));
        MOCK_METHOD1(Deserialize, void(infra::ProtoParser& parser));

    public:
        static const uint32_t maxMessageSize = 16;
    };
}

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
    MOCK_METHOD2(Write, void(std::size_t size, const infra::Function<void()>& onDone));
    MOCK_METHOD1(Erase, void(const infra::Function<void()>& onDone));
};

class ConfigurationStoreTest
    : public testing::Test
{
public:
    ConfigurationStoreTest()
        : configurationStore(configurationBlob1, configurationBlob2)
    {}

    MOCK_METHOD1(OnLoaded, void(bool success));

    void DontRecover()
    {
        EXPECT_CALL(configurationBlob1, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
        configurationStore.Recover([this](bool success) { OnLoaded(success); });

        EXPECT_CALL(configurationBlob2, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
        onRecoverDone(false);

        EXPECT_CALL(configurationBlob1, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
        onRecoverDone(false);

        EXPECT_CALL(*this, OnLoaded(false));
        onEraseDone();
    }

public:
    infra::Function<void(bool success)> onRecoverDone;
    infra::Function<void()> onEraseDone;
    testing::StrictMock<ConfigurationBlobMock> configurationBlob1;
    testing::StrictMock<ConfigurationBlobMock> configurationBlob2;
    services::ConfigurationStore<testing::StrictMock<Data>> configurationStore;
};

TEST_F(ConfigurationStoreTest, failed_blob_load_is_propagated)
{
    EXPECT_CALL(configurationBlob1, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    configurationStore.Recover([this](bool success) { OnLoaded(success); });

    EXPECT_CALL(configurationBlob2, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    onRecoverDone(false);

    EXPECT_CALL(configurationBlob1, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onRecoverDone(false);

    EXPECT_CALL(*this, OnLoaded(false));
    onEraseDone();
}

TEST_F(ConfigurationStoreTest, after_succesful_blob_load_configuration_is_available)
{
    EXPECT_CALL(configurationBlob1, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    configurationStore.Recover([this](bool success) { OnLoaded(success); });

    EXPECT_CALL(configurationBlob2, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onRecoverDone(true);

    EXPECT_CALL(configurationBlob1, CurrentBlob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(*this, OnLoaded(true));
    onEraseDone();
}

TEST_F(ConfigurationStoreTest, after_succesful_blob_load_from_second_flash_configuration_is_available)
{
    EXPECT_CALL(configurationBlob1, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    configurationStore.Recover([this](bool success) { OnLoaded(success); });

    EXPECT_CALL(configurationBlob2, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    onRecoverDone(false);

    EXPECT_CALL(configurationBlob1, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onRecoverDone(true);

    EXPECT_CALL(configurationBlob2, CurrentBlob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(*this, OnLoaded(true));
    onEraseDone();
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

    EXPECT_CALL(configurationBlob2, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onWriteDone();

    onEraseDone();
}

TEST_F(ConfigurationStoreTest, Write_writes_to_other_blob_than_recovered)
{
    EXPECT_CALL(configurationBlob1, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    configurationStore.Recover([this](bool success) { OnLoaded(success); });

    EXPECT_CALL(configurationBlob2, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onRecoverDone(true);

    EXPECT_CALL(configurationBlob1, CurrentBlob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(*this, OnLoaded(true));
    onEraseDone();

    infra::Function<void()> onWriteDone;
    std::array<uint8_t, 32> data;
    EXPECT_CALL(configurationBlob2, MaxBlob()).WillOnce(testing::Return(infra::MakeRange(data)));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_)).WillOnce(infra::Lambda([](infra::ProtoFormatter& formatter) { formatter.PutFixed32(1); }));
    EXPECT_CALL(configurationBlob2, Write(4, testing::_)).WillOnce(testing::SaveArg<1>(&onWriteDone));
    configurationStore.Write();

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

    EXPECT_CALL(configurationBlob2, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onWriteDone();

    onEraseDone();
}

class FactoryDefaultConfigurationStoreTest
    : public testing::Test
{
public:
    FactoryDefaultConfigurationStoreTest()
        : configurationStore(configurationBlobFactoryDefault, configurationBlob1, configurationBlob2)
    {}

    MOCK_METHOD0(OnLoadFactoryDefault, void());
    MOCK_METHOD0(OnRecovered, void());

public:
    infra::Function<void(bool success)> onRecoverDone;
    infra::Function<void()> onEraseDone;
    testing::StrictMock<ConfigurationBlobMock> configurationBlobFactoryDefault;
    testing::StrictMock<ConfigurationBlobMock> configurationBlob1;
    testing::StrictMock<ConfigurationBlobMock> configurationBlob2;
    services::FactoryDefaultConfigurationStore<testing::StrictMock<Data>> configurationStore;
};

TEST_F(FactoryDefaultConfigurationStoreTest, failed_factory_default_results_in_OnLoadFactoryDefault)
{
    EXPECT_CALL(configurationBlobFactoryDefault, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    configurationStore.Recover([this]() { OnLoadFactoryDefault(); }, [this]() { OnRecovered(); });

    EXPECT_CALL(*this, OnLoadFactoryDefault());
    EXPECT_CALL(configurationBlobFactoryDefault, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onRecoverDone(false);

    std::array<uint8_t, 32> data;
    EXPECT_CALL(configurationBlobFactoryDefault, MaxBlob()).WillOnce(testing::Return(infra::MakeRange(data)));
    EXPECT_CALL(configurationStore.Configuration(), Serialize(testing::_)).WillOnce(infra::Lambda([](infra::ProtoFormatter& formatter) { formatter.PutFixed32(1); }));
    infra::Function<void()> onWriteDone;
    EXPECT_CALL(configurationBlobFactoryDefault, Write(4, testing::_)).WillOnce(testing::SaveArg<1>(&onWriteDone));
    onEraseDone();

    EXPECT_CALL(configurationBlob2, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onWriteDone();

    EXPECT_CALL(configurationBlob1, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onEraseDone();

    EXPECT_CALL(*this, OnRecovered());
    onEraseDone();
}

TEST_F(FactoryDefaultConfigurationStoreTest, successful_factory_default_results_in_ConfigurationStore_Recover)
{
    EXPECT_CALL(configurationBlobFactoryDefault, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    configurationStore.Recover([this]() { OnLoadFactoryDefault(); }, [this]() { OnRecovered(); });

    EXPECT_CALL(configurationBlobFactoryDefault, CurrentBlob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(configurationBlob1, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    onRecoverDone(true);    // Factory default is recovered

    EXPECT_CALL(configurationBlob2, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onRecoverDone(true);    // Blob 1 is recovered

    EXPECT_CALL(configurationBlob1, CurrentBlob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(*this, OnRecovered());
    onEraseDone();
}

TEST_F(FactoryDefaultConfigurationStoreTest, when_ConfigurationStore_Recover_fails_no_additional_configuration_is_deserialized)
{
    EXPECT_CALL(configurationBlobFactoryDefault, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    configurationStore.Recover([this]() { OnLoadFactoryDefault(); }, [this]() { OnRecovered(); });

    EXPECT_CALL(configurationBlobFactoryDefault, CurrentBlob()).WillOnce(testing::Return(infra::ByteRange()));
    EXPECT_CALL(configurationStore.Configuration(), Deserialize(testing::_));
    EXPECT_CALL(configurationBlob1, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    onRecoverDone(true);    // Factory default is recovered

    EXPECT_CALL(configurationBlob2, Recover(testing::_)).WillOnce(testing::SaveArg<0>(&onRecoverDone));
    onRecoverDone(false);    // Blob 1 is not recovered

    EXPECT_CALL(configurationBlob1, Erase(testing::_)).WillOnce(testing::SaveArg<0>(&onEraseDone));
    onRecoverDone(false);    // Blob 2 is not recovered

    EXPECT_CALL(*this, OnRecovered());
    onEraseDone();
}

class FactoryDefaultConfigurationStoreIntegrationTest
    : public testing::Test
    , public infra::EventDispatcherFixture
{
public:
    FactoryDefaultConfigurationStoreIntegrationTest()
        : flashFactoryDefault(1, 32)
        , flashBlob1(1, 32)
        , flashBlob2(1, 32)
    {}

    void ConstructConfigurationStore()
    {
        configurationStore.Emplace(flashFactoryDefault, flashBlob1, flashBlob2, [this]() { OnLoadFactoryDefault(); }, [this]() { OnRecovered(); });
    }

    MOCK_METHOD0(OnLoadFactoryDefault, void());
    MOCK_METHOD0(OnRecovered, void());

    struct Data
    {
        void Serialize(infra::ProtoFormatter& formatter)
        {
            infra::BoundedVector<uint8_t>::WithMaxSize<8> bytes(data.begin(), data.end());
            formatter.PutBytesField(bytes, 1);
        }

        void Deserialize(infra::ProtoParser& parser)
        {
            infra::ProtoParser::Field field = parser.GetField();
            assert(field.second == 1);
            infra::BoundedVector<uint8_t>::WithMaxSize<8> bytes;
            field.first.Get<infra::ProtoLengthDelimited>().GetBytes(bytes);
            assert(bytes.size() == data.size());
            std::copy(bytes.begin(), bytes.end(), data.begin());
        }

    public:
        static const uint32_t maxMessageSize = 10;

        std::array<uint8_t, 8> data = { 0, 1, 2, 3, 4, 5, 6, 7 };
    };

public:
    hal::FlashStub flashFactoryDefault;
    hal::FlashStub flashBlob1;
    hal::FlashStub flashBlob2;
    infra::Optional<services::FactoryDefaultConfigurationStore<Data>::WithBlobs> configurationStore;
};

TEST_F(FactoryDefaultConfigurationStoreIntegrationTest, start_with_empty_flash_results_in_load_default)
{
    flashBlob1.sectors[0] = std::vector<uint8_t>{
        0x00, 0x00, 0xaa, 0xaa, 0xaa, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    flashBlob2.sectors[0] = std::vector<uint8_t>{
        0x00, 0x00, 0xaa, 0xaa, 0xaa, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x00, 0x00, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    EXPECT_CALL(*this, OnLoadFactoryDefault()).WillOnce(infra::Lambda([&]()
    {
        configurationStore->Configuration().data = std::array<uint8_t, 8>{ 4, 3, 2, 1, 8, 7, 6, 5 };
    }));

    EXPECT_CALL(*this, OnRecovered());

    ConstructConfigurationStore();
    ExecuteAllActions();

    EXPECT_EQ((std::array<uint8_t, 8>{ 4, 3, 2, 1, 8, 7, 6, 5 }), configurationStore->Configuration().data);
    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashFactoryDefault.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob1.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob2.sectors[0]);
}

TEST_F(FactoryDefaultConfigurationStoreIntegrationTest, start_with_corrupt_factory_default_results_in_load_default)
{
    flashFactoryDefault.sectors[0] = std::vector<uint8_t>{
        0x00, 0x00, 0xaa, 0xaa, 0xaa, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    EXPECT_CALL(*this, OnLoadFactoryDefault()).WillOnce(infra::Lambda([&]()
    {
        configurationStore->Configuration().data = std::array<uint8_t, 8>{ 4, 3, 2, 1, 8, 7, 6, 5 };
    }));

    EXPECT_CALL(*this, OnRecovered());

    ConstructConfigurationStore();
    ExecuteAllActions();

    EXPECT_EQ((std::array<uint8_t, 8>{ 4, 3, 2, 1, 8, 7, 6, 5 }), configurationStore->Configuration().data);
    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
            0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashFactoryDefault.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob1.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob2.sectors[0]);
}

TEST_F(FactoryDefaultConfigurationStoreIntegrationTest, factory_default_is_recovered_but_blobs_are_corrupt)
{
    flashFactoryDefault.sectors[0] = std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    flashBlob1.sectors[0] = std::vector<uint8_t>{
        0x00, 0x00, 0xaa, 0xaa, 0xaa, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    flashBlob2.sectors[0] = std::vector<uint8_t>{
        0x00, 0x00, 0xaa, 0xaa, 0xaa, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x00, 0x00, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    EXPECT_CALL(*this, OnRecovered());

    ConstructConfigurationStore();
    ExecuteAllActions();

    EXPECT_EQ((std::array<uint8_t, 8>{ 4, 3, 2, 1, 8, 7, 6, 5 }), configurationStore->Configuration().data);
    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashFactoryDefault.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob1.sectors[0]);
    // flashBlob2 is not erased, because it's both corrupt and the inactive blob. Once blob1 is written, blob2 will get erased.

    configurationStore->Configuration().data = std::array<uint8_t, 8>{ 1, 3, 5, 7, 9, 2, 4, 6 };
    configurationStore->Write();
    ExecuteAllActions();

    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashFactoryDefault.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0x19, 0x4c, 0xec, 0x8b, 0xcd, 0x3a, 0x14, 0x57, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x01, 0x03,
        0x05, 0x07, 0x09, 0x02, 0x04, 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob1.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob2.sectors[0]);
}

TEST_F(FactoryDefaultConfigurationStoreIntegrationTest, both_blobs_are_correct_so_second_is_erased)
{
    flashFactoryDefault.sectors[0] = std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    flashBlob1.sectors[0] = std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    flashBlob2.sectors[0] = std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    EXPECT_CALL(*this, OnRecovered());

    ConstructConfigurationStore();
    ExecuteAllActions();

    EXPECT_EQ((std::array<uint8_t, 8>{ 4, 3, 2, 1, 8, 7, 6, 5 }), configurationStore->Configuration().data);
    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashFactoryDefault.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob1.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob2.sectors[0]);

    configurationStore->Configuration().data = std::array<uint8_t, 8>{ 1, 3, 5, 7, 9, 2, 4, 6 };
    configurationStore->Write();
    ExecuteAllActions();

    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashFactoryDefault.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob1.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0x19, 0x4c, 0xec, 0x8b, 0xcd, 0x3a, 0x14, 0x57, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x01, 0x03,
        0x05, 0x07, 0x09, 0x02, 0x04, 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob2.sectors[0]);
}

TEST_F(FactoryDefaultConfigurationStoreIntegrationTest, second_blob_is_recovered)
{
    flashFactoryDefault.sectors[0] = std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    flashBlob1.sectors[0] = std::vector<uint8_t>{
        0x00, 0x00, 0x00, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x00, 0x00, 0x00, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    flashBlob2.sectors[0] = std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    EXPECT_CALL(*this, OnRecovered());

    ConstructConfigurationStore();
    ExecuteAllActions();

    EXPECT_EQ((std::array<uint8_t, 8>{ 4, 3, 2, 1, 8, 7, 6, 5 }), configurationStore->Configuration().data);
    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashFactoryDefault.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob1.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob2.sectors[0]);

    configurationStore->Configuration().data = std::array<uint8_t, 8>{ 1, 3, 5, 7, 9, 2, 4, 6 };
    configurationStore->Write();
    ExecuteAllActions();

    EXPECT_EQ((std::vector<uint8_t>{
        0xfb, 0x76, 0x16, 0x1c, 0x5a, 0x51, 0x1f, 0xfe, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x04, 0x03,
        0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashFactoryDefault.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0x19, 0x4c, 0xec, 0x8b, 0xcd, 0x3a, 0x14, 0x57, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x01, 0x03,
        0x05, 0x07, 0x09, 0x02, 0x04, 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob1.sectors[0]);
    EXPECT_EQ((std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }), flashBlob2.sectors[0]);
}
