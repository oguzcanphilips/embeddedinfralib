#ifndef SERVICES_CONFIGURATION_STORE_HPP
#define SERVICES_CONFIGURATION_STORE_HPP

#include "hal/interfaces/Flash.hpp"
#include "infra/stream/ByteInputStream.hpp"
#include "infra/stream/ByteOutputStream.hpp"
#include "infra/syntax/ProtoFormatter.hpp"
#include "infra/syntax/ProtoParser.hpp"
#include "infra/util/AutoResetFunction.hpp"
#include "infra/util/WithStorage.hpp"

namespace services
{
    class ConfigurationBlob
    {
    protected:
        ConfigurationBlob() = default;
        ConfigurationBlob(const ConfigurationBlob& other) = delete;
        ConfigurationBlob& operator=(const ConfigurationBlob& other) = delete;
        ~ConfigurationBlob() = default;

    public:
        virtual infra::ByteRange CurrentBlob() = 0;
        virtual infra::ByteRange MaxBlob() = 0;
        virtual void Recover(const infra::Function<void(bool success)>& onRecovered) = 0;
        virtual void Write(uint32_t size, const infra::Function<void()>& onDone) = 0;
        virtual void Erase(const infra::Function<void()>& onDone) = 0;
    };

    class ConfigurationBlobImpl
        : public ConfigurationBlob
    {
    private:
        struct Header
        {
            std::array<uint8_t, 8> hash;
            uint32_t size;
        };

    public:
        template<std::size_t Size>
            using WithStorage = infra::WithStorage<ConfigurationBlobImpl, std::array<uint8_t, Size + sizeof(Header)>>;

        ConfigurationBlobImpl(infra::ByteRange blob, hal::Flash& flash);

        virtual infra::ByteRange CurrentBlob() override;
        virtual infra::ByteRange MaxBlob() override;
        virtual void Recover(const infra::Function<void(bool success)>& onRecovered) override;
        virtual void Write(uint32_t size, const infra::Function<void()>& onDone) override;
        virtual void Erase(const infra::Function<void()>& onDone) override;

    private:
        void RecoverCurrentSize();
        bool BlobIsValid() const;
        void PrepareBlobForWriting();

    private:
        infra::ByteRange blob;
        hal::Flash& flash;
        uint32_t currentSize = 0;
        infra::AutoResetFunction<void(bool success)> onRecovered;
    };

    class ConfigurationStoreBase
    {
    public:
        ConfigurationStoreBase(ConfigurationBlob& blob1, ConfigurationBlob& blob2);

    public:
        void Recover(const infra::Function<void(bool success)>& onRecovered);
        void Write(infra::Function<void()> onDone = infra::Function<void()>());
        void Erase(infra::Function<void()> onDone);

        virtual void Serialize(ConfigurationBlob& blob, const infra::Function<void()>& onDone) = 0;
        virtual void Deserialize(ConfigurationBlob& blob) = 0;

        class LockGuard
        {
        public:
            LockGuard(ConfigurationStoreBase& store);
            LockGuard(const LockGuard& other);
            LockGuard& operator=(const LockGuard& other);
            ~LockGuard();

        private:
            ConfigurationStoreBase* store;
        };

        LockGuard Lock();

    private:
        void OnBlobLoaded(bool success);
        void BlobWriteDone();
        void Unlocked();

    private:
        ConfigurationBlob* activeBlob;
        ConfigurationBlob* inactiveBlob;
        infra::AutoResetFunction<void()> onWriteDone;
        infra::AutoResetFunction<void(bool success)> onRecovered;
        uint32_t lockCount = 0;
        bool writingBlob = false;
        bool writeRequested = false;
    };

    template<class T>
    class ConfigurationStore
        : public ConfigurationStoreBase
    {
    public:
        class WithBlobs;

        ConfigurationStore(ConfigurationBlob& blob1, ConfigurationBlob& blob2);

        const T& Configuration() const;
        T& Configuration();

        virtual void Serialize(ConfigurationBlob& blob, const infra::Function<void()>& onDone) override;
        virtual void Deserialize(ConfigurationBlob& blob) override;

    private:
        T configuration;
    };

    template<class T>
    class ConfigurationStore<T>::WithBlobs
        : public ConfigurationStore<T>
    {
    public:
        WithBlobs(hal::Flash& flashFirst, hal::Flash& flashSecond, const infra::Function<void(bool success)>& onRecovered);

    private:
        typename ConfigurationBlobImpl::WithStorage<T::maxMessageSize> blob1;
        ConfigurationBlobImpl blob2;
    };

    class FactoryDefaultConfigurationStoreBase
    {
    public:
        FactoryDefaultConfigurationStoreBase(ConfigurationStoreBase& configurationStore, ConfigurationBlob& factoryDefaultBlob);

        void Recover(const infra::Function<void()>& onLoadFactoryDefault, const infra::Function<void(bool isFactoryDefault)>& onRecovered);
        void Write(infra::Function<void()> onDone = infra::Function<void()>());

    private:
        ConfigurationStoreBase& configurationStore;
        ConfigurationBlob& factoryDefaultBlob;
        infra::AutoResetFunction<void()> onLoadFactoryDefault;
        infra::AutoResetFunction<void(bool isFactoryDefault)> onRecovered;
    };

    template<class T>
    class FactoryDefaultConfigurationStore
        : public FactoryDefaultConfigurationStoreBase
    {
    public:
        class WithBlobs;

        FactoryDefaultConfigurationStore(ConfigurationBlob& blobFactoryDefault, ConfigurationBlob& blob1, ConfigurationBlob& blob2);

        const T& Configuration() const;
        T& Configuration();

    private:
        ConfigurationStore<T> configurationStore;
    };

    template<class T>
    class FactoryDefaultConfigurationStore<T>::WithBlobs
        : public FactoryDefaultConfigurationStore<T>
    {
    public:
        WithBlobs(hal::Flash& flashFactoryDefault, hal::Flash& flashFirst, hal::Flash& flashSecond
            , const infra::Function<void()>& onLoadFactoryDefault, const infra::Function<void(bool isFactoryDefault)>& onRecovered);

    private:
        ConfigurationBlobImpl::WithStorage<T::maxMessageSize> factoryDefaultBlob;
        ConfigurationBlobImpl blob1;
        ConfigurationBlobImpl blob2;
    };

    ////    Implementation    ////

    template<class T>
    ConfigurationStore<T>::ConfigurationStore(ConfigurationBlob& blob1, ConfigurationBlob& blob2)
        : ConfigurationStoreBase(blob1, blob2)
    {}

    template<class T>
    const T& ConfigurationStore<T>::Configuration() const
    {
        return configuration;
    }

    template<class T>
    T& ConfigurationStore<T>::Configuration()
    {
        return configuration;
    }

    template<class T>
    void ConfigurationStore<T>::Serialize(ConfigurationBlob& blob, const infra::Function<void()>& onDone)
    {
        infra::ByteOutputStream stream(blob.MaxBlob());
        infra::ProtoFormatter formatter(stream);
        configuration.Serialize(formatter);
        blob.Write(stream.Writer().Processed().size(), onDone);
    }

    template<class T>
    void ConfigurationStore<T>::Deserialize(ConfigurationBlob& blob)
    {
        infra::ByteInputStream stream(blob.CurrentBlob());
        infra::ProtoParser parser(stream);
        configuration.Deserialize(parser);
    }

    template<class T>
    ConfigurationStore<T>::WithBlobs::WithBlobs(hal::Flash& flashFirst, hal::Flash& flashSecond, const infra::Function<void(bool success)>& onRecovered)
        : ConfigurationStore<T>(this->blob1, this->blob2, onRecovered)
        , blob1(flashFirst, Configuration())
        , blob2(blob1.Storage(), flashSecond, Configuration())
    {
        Recover(onRecovered);
    }

    template<class T>
    FactoryDefaultConfigurationStore<T>::FactoryDefaultConfigurationStore(ConfigurationBlob& blobFactoryDefault, ConfigurationBlob& blob1, ConfigurationBlob& blob2)
        : FactoryDefaultConfigurationStoreBase(configurationStore, blobFactoryDefault)
        , configurationStore(blob1, blob2)
    {}

    template<class T>
    const T& FactoryDefaultConfigurationStore<T>::Configuration() const
    {
        return configurationStore.Configuration();
    }

    template<class T>
    T& FactoryDefaultConfigurationStore<T>::Configuration()
    {
        return configurationStore.Configuration();
    }

    template<class T>
    FactoryDefaultConfigurationStore<T>::WithBlobs::WithBlobs(hal::Flash& flashFactoryDefault, hal::Flash& flashFirst, hal::Flash& flashSecond
        , const infra::Function<void()>& onLoadFactoryDefault, const infra::Function<void(bool isFactoryDefault)>& onRecovered)
        : FactoryDefaultConfigurationStore(factoryDefaultBlob, blob1, blob2)
        , factoryDefaultBlob(flashFactoryDefault)
        , blob1(factoryDefaultBlob.Storage(), flashFirst)
        , blob2(factoryDefaultBlob.Storage(), flashSecond)
    {
        Recover(onLoadFactoryDefault, onRecovered);
    }
}

#endif
