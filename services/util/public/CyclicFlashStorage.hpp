#ifndef SERVICES_CYCLIC_FLASH_STORAGE_HPP
#define SERVICES_CYCLIC_FLASH_STORAGE_HPP

/*#include "hal_interface/Flash.hpp"
#include "infra_event/ClaimableResource.hpp"
#include "infra_sequencer/Sequencer.hpp"
#include "infra_util/IntrusiveForwardList.hpp"


namespace services
{

    class CyclicFlashStorage
    {
    public:
        CyclicFlashStorage(hal::Flash& flash, infra::Gate& onFlashInitialized);
        CyclicFlashStorage(const CyclicFlashStorage& other) = delete;
        CyclicFlashStorage& operator=(const CyclicFlashStorage& other) = delete;

        void Add(infra::ConstByteRange range, infra::Function<void()> onDone);
        void Clear(infra::Function<void()> onDone);

        class Iterator;

        Iterator Begin() const;

    private:
        void Recover();
        infra::Sequencer& RecoverSector(uint32_t sectorIndex);
        infra::Sequencer& RecoverEndAddress();

        infra::Sequencer& EraseSectorIfAtStart();
        infra::Sequencer& FillSectorIfDataDoesNotFit(std::size_t size);
        infra::Sequencer& WriteSectorStatusIfAtStartOfSector();
        infra::Sequencer& WriteRange(infra::ConstByteRange range);

    private:
        hal::Flash& flash;
        infra::Gate& onFlashInitialized;
        uint32_t startAddress;
        uint32_t endAddress;
        infra::Sequencer sequencer;
        infra::Sequencer nestedSequencer;
        mutable infra::ClaimableResource resource;
        infra::ClaimableResource::Claimer::WithSize<12> claimer;
        infra::ClaimableResource::Claimer::WithSize<12> recoverClaimer;
        infra::AutoResetFunction<void()> onAddDone;
        infra::AutoResetFunction<void()> onClearDone;

        using Length = uint16_t;

        enum class BlockStatus: uint8_t
        {
            empty = 0xff,
            emptyUntilEnd = 0x7f,
            writingLength = 0xfe,
            writingData = 0xfc,
            dataReady = 0xf8
        };

        enum class SectorStatus: uint8_t
        {
            empty = 0xff,
            used = 0xfe,
            firstInCycle = 0xfc
        };

        enum class RecoverPhase: uint8_t
        {
            searchingStartOrEmpty,
            checkingAllUsed,
            checkingUsedFollowedByEmpty,
            checkingAllEmpty,
            checkingAllUsedOrAllEmpty,
            corrupt,
            done
        };

        struct BlockHeader
        {
            BlockStatus status;
            uint8_t lengthLsb;
            uint8_t lengthMsb;

            Length BlockLength() const
            {
                return lengthLsb + (static_cast<Length>(lengthMsb) << 8);
            }

            void SetBlockLength(Length length)
            {
                lengthLsb = static_cast<uint8_t>(length);
                lengthMsb = static_cast<uint8_t>(length >> 8);
            }
        };

        SectorStatus sectorStatus;
        BlockHeader blockHeader;
        RecoverPhase recoverPhase = RecoverPhase::searchingStartOrEmpty;

        mutable infra::IntrusiveForwardList<Iterator> iterators;
    };

    class CyclicFlashStorage::Iterator
        : public infra::IntrusiveForwardList<Iterator>::NodeType
    {
    public:
        explicit Iterator(const CyclicFlashStorage& store);
        Iterator(const Iterator& other);
        ~Iterator();
        Iterator& operator=(const Iterator& other);

        void Read(infra::ByteRange buffer, infra::Function<void(infra::ByteRange result)> onDone);

        void SectorIsErased(uint32_t sectorIndex);

    private:
        infra::Sequencer& ReadSectorStatusIfAtStart();

    private:
        const CyclicFlashStorage& store;
        bool loadStartAddressDelayed;
        uint32_t address;
        infra::Sequencer sequencer;
        infra::Sequencer nestedSequencer;
        infra::ClaimableResource::Claimer::WithSize<INFRA_DEFAULT_FUNCTION_EXTRA_SIZE + 8> claimer;

        SectorStatus sectorStatus = SectorStatus::used;
        bool firstSectorToRead = true;
        BlockHeader blockHeader;
        infra::ByteRange readBuffer;

        bool found;
    };

}*/

#endif
