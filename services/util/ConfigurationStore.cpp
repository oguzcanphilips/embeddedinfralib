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
        really_assert(blob.size() <= flashFirst.TotalSize());
        really_assert(blob.size() <= flashSecond.TotalSize());

        Recover();
    }

    infra::ByteRange ConfigurationBlobImpl::CurrentBlob()
    {
        return infra::Head(infra::DiscardHead(blob, sizeof(Header)), currentSize);
    }

    infra::ByteRange ConfigurationBlobImpl::MaxBlob()
    {
        return infra::DiscardHead(blob, sizeof(Header));
    }

    void ConfigurationBlobImpl::Write(uint32_t size, const infra::Function<void()>& onDone)
    {
        onWriteDone = onDone;
        currentSize = size;
        PrepareBlobForWriting();
        std::swap(inactiveFlash, activeFlash);
        activeFlash->WriteBuffer(blob, 0, [this]() { inactiveFlash->EraseAll([this]() { onWriteDone(); }); });
    }

    void ConfigurationBlobImpl::Recover()
    {
        activeFlash->ReadBuffer(blob, 0, [this]()
        {
            if (BlobIsValid())
            {
                RecoverCurrentSize();
                EraseInactiveFlashAfterRecovery(true);
            }
            else
            {
                std::swap(activeFlash, inactiveFlash);
                activeFlash->ReadBuffer(blob, 0, [this]()
                {
                    if (BlobIsValid())
                    {
                        RecoverCurrentSize();
                        EraseInactiveFlashAfterRecovery(true);
                    }
                    else
                        EraseInactiveFlashAfterRecovery(false);
                });
            }
        });
    }

    void ConfigurationBlobImpl::RecoverCurrentSize()
    {
        Header header;
        infra::Copy(infra::Head(blob, sizeof(header)), infra::MakeByteRange(header));

        currentSize = header.size;
    }

    void ConfigurationBlobImpl::EraseInactiveFlashAfterRecovery(bool success)
    {
        inactiveFlash->EraseAll([this, success]()
        {
            onLoaded(success);
        });
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
            infra::ByteOutputStream stream(blob.MaxBlob());
            infra::ProtoFormatter formatter(stream);
            Serialize(formatter);
            blob.Write(stream.Writer().Processed().size(), [this]() { BlobWriteDone(); });
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
            infra::ByteInputStream stream(blob.CurrentBlob());
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
