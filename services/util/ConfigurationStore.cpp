#include "mbedtls/sha256.h"
#include "services/util/ConfigurationStore.hpp"

namespace services
{
    ConfigurationBlobImpl::ConfigurationBlobImpl(infra::ByteRange blob, hal::Flash& flash)
        : blob(blob)
        , flash(flash)
    {
        really_assert(blob.size() <= flash.TotalSize());
    }

    infra::ByteRange ConfigurationBlobImpl::CurrentBlob()
    {
        return infra::Head(infra::DiscardHead(blob, sizeof(Header)), currentSize);
    }

    infra::ByteRange ConfigurationBlobImpl::MaxBlob()
    {
        return infra::DiscardHead(blob, sizeof(Header));
    }

    void ConfigurationBlobImpl::Recover(const infra::Function<void(bool success)>& onLoaded)
    {
        this->onLoaded = onLoaded;
        flash.ReadBuffer(blob, 0, [this]()
        {
            if (BlobIsValid())
            {
                RecoverCurrentSize();
                this->onLoaded(true);
            }
            else
                this->onLoaded(false);
        });
    }

    void ConfigurationBlobImpl::Erase(const infra::Function<void()>& onDone)
    {
        flash.EraseAll(onDone);
    }

    void ConfigurationBlobImpl::Write(uint32_t size, const infra::Function<void()>& onDone)
    {
        currentSize = size;
        PrepareBlobForWriting();
        flash.WriteBuffer(blob, 0, onDone);
    }

    void ConfigurationBlobImpl::RecoverCurrentSize()
    {
        Header header;
        infra::Copy(infra::Head(blob, sizeof(header)), infra::MakeByteRange(header));

        currentSize = header.size;
    }

    bool ConfigurationBlobImpl::BlobIsValid() const
    {
        Header header;
        infra::Copy(infra::Head(blob, sizeof(header)), infra::MakeByteRange(header));

        if (header.size + sizeof(Header) > blob.size())
            return false;

        std::array<uint8_t, 32> messageHash;
        mbedtls2_sha256(blob.begin() + sizeof(header.hash), std::min<std::size_t>(header.size + sizeof(header.size), blob.size() - sizeof(header.hash)), messageHash.data(), 0);  //TICS !INT#030

        return infra::Head(infra::MakeRange(messageHash), sizeof(header.hash)) == header.hash;
    }

    void ConfigurationBlobImpl::PrepareBlobForWriting()
    {
        Header header;
        header.size = currentSize;
        infra::Copy(infra::MakeByteRange(header), infra::Head(blob, sizeof(header)));

        std::array<uint8_t, 32> messageHash;
        mbedtls2_sha256(blob.begin() + sizeof(header.hash), currentSize + sizeof(header.size), messageHash.data(), 0);  //TICS !INT#030

        infra::Copy(infra::Head(infra::MakeRange(messageHash), sizeof(header.hash)), infra::MakeRange(header.hash));
        infra::Copy(infra::MakeByteRange(header), infra::Head(blob, sizeof(header)));
    }

    ConfigurationStoreBase::ConfigurationStoreBase(ConfigurationBlob& blob1, ConfigurationBlob& blob2)
        : activeBlob(&blob1)
        , inactiveBlob(&blob2)
    {}

    void ConfigurationStoreBase::Write(infra::Function<void()> onDone)
    {
        if (onDone)
            onWriteDone = onDone;

        writeRequested = true;
        if (!writingBlob && lockCount == 0)
        {
            writeRequested = false;
            writingBlob = true;
            Serialize(*activeBlob, [this]() { inactiveBlob->Erase([this]() { BlobWriteDone(); }); });
        }
    }

    ConfigurationStoreBase::LockGuard ConfigurationStoreBase::Lock()
    {
        return LockGuard(*this);
    }

    void ConfigurationStoreBase::Recover()
    {
        activeBlob->Recover([this](bool success)
        {
            if (success)
                OnBlobLoaded(true);
            else
            {
                std::swap(activeBlob, inactiveBlob);
                activeBlob->Recover([this](bool success)
                {
                    OnBlobLoaded(success);
                });
            }
        });
    }

    void ConfigurationStoreBase::OnBlobLoaded(bool success)
    {
        std::swap(activeBlob, inactiveBlob);

        if (success)
            Deserialize(*inactiveBlob);

        OnLoaded(success);
    }

    void ConfigurationStoreBase::BlobWriteDone()
    {
        std::swap(activeBlob, inactiveBlob);
        writingBlob = false;
        if (writeRequested)
            Write();
        else if (onWriteDone)
            onWriteDone();
    }

    void ConfigurationStoreBase::Unlocked()
    {
        if (writeRequested)
            Write();
    }

    ConfigurationStoreBase::LockGuard::LockGuard(ConfigurationStoreBase& store)
        : store(&store)
    {
        ++store.lockCount;
    }

    ConfigurationStoreBase::LockGuard::LockGuard(const LockGuard& other)
        : store(other.store)
    {
        ++store->lockCount;
    }

    ConfigurationStoreBase::LockGuard& ConfigurationStoreBase::LockGuard::operator=(const LockGuard& other)
    {
        if (this != &other)
        {
            --store->lockCount;
            if (store->lockCount == 0)
                store->Unlocked();

            store = other.store;
            ++store->lockCount;
        }

        return *this;
    }

    ConfigurationStoreBase::LockGuard::~LockGuard()
    {
        --store->lockCount;
        if (store->lockCount == 0)
            store->Unlocked();
    }
}
