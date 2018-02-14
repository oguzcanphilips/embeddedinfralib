#include "infra/stream/ByteInputStream.hpp"
#include "infra/stream/ByteOutputStream.hpp"
#include "mbedtls/sha256.h"
#include "services/util/ConfigurationStore.hpp"

namespace services
{
    ConfigurationBlobImpl::ConfigurationBlobImpl(infra::ByteRange blob, hal::Flash& flashFirst, hal::Flash& flashSecond, const infra::Function<void(bool success)>& onLoaded)
        : blob(blob)
        , activeFlash(&flashFirst)
        , inactiveFlash(&flashSecond)
        , onLoaded(onLoaded)
    {
        Recover();
    }

    infra::ByteRange ConfigurationBlobImpl::Blob()
    {
        return infra::DiscardHead(blob, sizeof(Header));
    }

    void ConfigurationBlobImpl::Write(const infra::Function<void()>& onDone)
    {
        onWriteDone = onDone;
        PrepareBlobForWriting();
        std::swap(inactiveFlash, activeFlash);
        activeFlash->WriteBuffer(blob, 0, [this]() { onWriteDone(); });
    }

    void ConfigurationBlobImpl::Recover()
    {
        activeFlash->ReadBuffer(blob, 0, [this]()
        {
            if (BlobIsValid())
                EraseInactiveFlashAfterRecovery();
            else
            {
                std::swap(activeFlash, inactiveFlash);
                activeFlash->ReadBuffer(blob, 0, [this]()
                {
                    if (BlobIsValid())
                        EraseInactiveFlashAfterRecovery();
                    else
                        onLoaded(false);
                });
            }
        });
    }

    void ConfigurationBlobImpl::EraseInactiveFlashAfterRecovery()
    {
        inactiveFlash->EraseAll([this]()
        {
            onLoaded(true);
        });
    }

    bool ConfigurationBlobImpl::BlobIsValid() const
    {
        infra::ByteInputStream stream(blob);
        Header header;
        stream >> header;

        if (header.size + sizeof(Header) > blob.size())
            return false;

        std::array<uint8_t, 32> messageHash;
        mbedtls2_sha256(blob.begin() + sizeof(header.hash), blob.size() - sizeof(header.hash), messageHash.data(), 0);  //TICS !INT#030

        return infra::Head(infra::MakeRange(messageHash), sizeof(header.hash)) == header.hash;
    }

    void ConfigurationBlobImpl::PrepareBlobForWriting()
    {
        Header header;
        {
            infra::ByteOutputStream stream(blob);
            header.size = blob.size() - sizeof(header);
            stream << header;
        }

        std::array<uint8_t, 32> messageHash;
        mbedtls2_sha256(blob.begin() + sizeof(header.hash), blob.size() - sizeof(header.hash), messageHash.data(), 0);  //TICS !INT#030

        infra::Copy(infra::Head(infra::MakeRange(messageHash), sizeof(header.hash)), infra::MakeRange(header.hash));

        {
            infra::ByteOutputStream stream(blob);
            header.size = blob.size() - sizeof(header);
            stream << header;
        }
    }

    ConfigurationStoreBase::ConfigurationStoreBase(ConfigurationBlob& blob, const infra::Function<void(bool success)>& onLoaded)
        : blob(blob)
        , onLoaded(onLoaded)
    {}

    void ConfigurationStoreBase::Write(infra::Function<void()> onDone)
    {
        assert(!onLoaded);
        if (onDone)
            onWriteDone = onDone;

        writeRequested = true;
        if (!writingBlob && lockCount == 0)
        {
            writeRequested = false;
            writingBlob = true;
            infra::ByteOutputStream stream(blob.Blob());
            infra::ProtoFormatter formatter(stream);
            Serialize(formatter);
            blob.Write([this]() { BlobWriteDone(); });
        }
    }

    ConfigurationStoreBase::LockGuard ConfigurationStoreBase::Lock()
    {
        return LockGuard(*this);
    }

    void ConfigurationStoreBase::OnBlobLoaded(bool success)
    {
        if (success)
        {
            infra::ByteInputStream stream(blob.Blob());
            infra::ProtoParser parser(stream);
            Deserialize(parser);
        }

        onLoaded(success);
    }

    void ConfigurationStoreBase::BlobWriteDone()
    {
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
