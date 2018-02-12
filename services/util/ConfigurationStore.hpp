#ifndef SERVICES_CONFIGURATION_STORE_HPP
#define SERVICES_CONFIGURATION_STORE_HPP

#include "hal/interfaces/Flash.hpp"
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
        virtual infra::ByteRange Blob() = 0;
        virtual void Write(const infra::Function<void()>& onDone) = 0;
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

        ConfigurationBlobImpl(infra::ByteRange blob, hal::Flash& flashFirst, hal::Flash& flashSecond, const infra::Function<void(bool success)>& onLoaded);

        virtual infra::ByteRange Blob() override;
        virtual void Write(const infra::Function<void()>& onDone) override;

    private:
        void Recover();
        void EraseInactiveFlashAfterRecovery();
        bool BlobIsValid() const;
        void PrepareBlobForWriting();

    private:
        infra::ByteRange blob;
        hal::Flash* activeFlash;
        hal::Flash* inactiveFlash;
        infra::AutoResetFunction<void(bool success)> onLoaded;
        infra::AutoResetFunction<void()> onWriteDone;
    };

    class ConfigurationStoreBase
    {
    public:
        ConfigurationStoreBase(ConfigurationBlob& blob, const infra::Function<void(bool success)>& onLoaded);

    public:
        void Write();

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
        void OnBlobLoaded(bool success);

    protected:
        virtual void Serialize(infra::ProtoFormatter& formatter) = 0;
        virtual void Deserialize(infra::ProtoParser& parser) = 0;

    private:
        void BlobWriteDone();
        void Unlocked();

    private:
        ConfigurationBlob& blob;
        infra::AutoResetFunction<void(bool success)> onLoaded;
        uint32_t lockCount = 0;
        bool writingBlob = false;
        bool writeRequested = false;
    };

    template<class T>
    class ConfigurationStore
        : public ConfigurationStoreBase
    {
    public:
        class WithBlob
            : public ConfigurationStore<T>
        {
        public:
            WithBlob(hal::Flash& flash1, hal::Flash& flash2, const infra::Function<void(bool success)>& onLoaded);

        private:
            ConfigurationBlobImpl::WithStorage<T::maxMessageSize> blob;
        };

        ConfigurationStore(ConfigurationBlob& blob, const infra::Function<void(bool success)>& onLoaded);

        const T& Configuration() const;
        T& Configuration();

    protected:
        virtual void Serialize(infra::ProtoFormatter& formatter) override;
        virtual void Deserialize(infra::ProtoParser& parser) override;

    private:
        T configuration;
    };

    ////    Implementation    ////

    template<class T>
    ConfigurationStore<T>::WithBlob::WithBlob(hal::Flash& flash1, hal::Flash& flash2, const infra::Function<void(bool success)>& onLoaded)
        : ConfigurationStore<T>(blob, onLoaded)
        , blob(flash1, flash2, [this](bool success) { this->onBlobLoaded(success); })
    {}

    template<class T>
    ConfigurationStore<T>::ConfigurationStore(ConfigurationBlob& blob, const infra::Function<void(bool success)>& onLoaded)
        : ConfigurationStoreBase(blob, onLoaded)
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
    void ConfigurationStore<T>::Serialize(infra::ProtoFormatter& formatter)
    {
        configuration.Serialize(formatter);
    }

    template<class T>
    void ConfigurationStore<T>::Deserialize(infra::ProtoParser& parser)
    {
        configuration.Deserialize(parser);
    }
 }

#endif
