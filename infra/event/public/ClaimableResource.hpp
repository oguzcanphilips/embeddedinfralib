#ifndef INFRA_CLAIMABLE_RESOURCE_HPP
#define INFRA_CLAIMABLE_RESOURCE_HPP

#include <cstdint>
#include "infra/util/public/AutoResetFunction.hpp"
#include "infra/util/public/IntrusiveList.hpp"

namespace infra
{
    class ClaimableResource
    {
    private:
        class ClaimerBase;

        template<std::size_t ExtraSize>
        class ClaimerWithSize;

    public:
        using Claimer = ClaimerWithSize<INFRA_DEFAULT_FUNCTION_EXTRA_SIZE>;

        ClaimableResource() = default;
        ClaimableResource(const ClaimableResource& other) = delete;
        ClaimableResource& operator=(const ClaimableResource& other) = delete;
        ~ClaimableResource();

    private:
        void ReEvaluateClaim();
        void AddClaim(ClaimerBase& claimer);
        void RemoveClaim(ClaimerBase& claimer);
        void EnqueueClaimer(ClaimerBase& claimer);
        void DequeueClaimer(ClaimerBase& claimer);

    private:
        infra::IntrusiveList<ClaimerBase> pendingClaims;
        ClaimerBase* currentClaim = nullptr;
    };

    class ClaimableResource::ClaimerBase
        : public infra::IntrusiveList<ClaimerBase>::NodeType
    {
    public:
        ClaimerBase(ClaimableResource& resource);
        ClaimerBase(const ClaimerBase& other) = delete;
        ~ClaimerBase();
        ClaimerBase& operator=(const ClaimerBase& other) = delete;

    public:
        virtual void Release() = 0;
        bool IsClaimed() const;

    private:
        friend class ClaimableResource;

        virtual void ClaimGranted() = 0;

    protected:
        ClaimableResource& resource;
        bool isGranted = false;
        bool isQueued = false;
    };

    template<std::size_t ExtraSize>
    class ClaimableResource::ClaimerWithSize
        : public ClaimerBase
    {
    public:
        template<std::size_t NewExtraSize>
            using WithSize = ClaimerWithSize<NewExtraSize>;

        ClaimerWithSize(ClaimableResource& resource);

        void Claim(const infra::Function<void(), ExtraSize>& claimedFunc);
        void Release() override;

    private:
        void ClaimGranted() override;

    private:
        infra::AutoResetFunction<void(), ExtraSize> onGranted;
    };

    ////    Implementation    ////

    template<std::size_t ExtraSize>
    ClaimableResource::ClaimerWithSize<ExtraSize>::ClaimerWithSize(ClaimableResource& resource)
        : ClaimerBase(resource)
    {}

    template<std::size_t ExtraSize>
    void ClaimableResource::ClaimerWithSize<ExtraSize>::Claim(const infra::Function<void(), ExtraSize>& onGranted)
    {
        assert(!this->onGranted);

        this->onGranted = onGranted;
        this->isQueued = true;
        resource.AddClaim(*this);
    }

    template<std::size_t ExtraSize>
    void ClaimableResource::ClaimerWithSize<ExtraSize>::Release()
    {
        if (!IsClaimed())
            this->onGranted = nullptr;

        ClaimerBase::Release();
    }

    template<std::size_t ExtraSize>
    void ClaimableResource::ClaimerWithSize<ExtraSize>::ClaimGranted()
    {
        isQueued = false;
        isGranted = true;
        onGranted();
    }
}

#endif