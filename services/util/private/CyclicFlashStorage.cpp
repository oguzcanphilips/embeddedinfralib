#include "services/util/public/CyclicFlashStorage.hpp"
/*
namespace services
{

    CyclicFlashStorage::CyclicFlashStorage(hal::Flash& flash, infra::Gate& onFlashInitialized)
        : flash(flash)
        , onFlashInitialized(onFlashInitialized)
        , claimer(resource)
        , recoverClaimer(resource)
    {
        Recover();
    }

    void CyclicFlashStorage::Add(infra::ConstByteRange range, infra::Function<void()> onDone)
    {
        assert(!range.empty());

        onAddDone = onDone;
        claimer.Claim([this, range]()
        {
            assert(sequencer.Finished());
            sequencer.Load([this, range]()
            {
                sequencer.Sequence(),
                infra::Nest(
                    [this, range]()->infra::Sequencer& { return EraseSectorIfAtStart(); }
                ),
                infra::Nest(
                    [this, range]()->infra::Sequencer& { return FillSectorIfDataDoesNotFit(range.size()); }
                ),
                infra::Nest(
                    [this]()->infra::Sequencer& { return WriteSectorStatusIfAtStartOfSector(); }
                ),
                infra::Nest(
                    [this, range]()->infra::Sequencer& { return WriteRange(range); }
                ),
                [this]()
                {
                    claimer.Release();
                    onAddDone();
                };
            });
        });
    }

    void CyclicFlashStorage::Clear(infra::Function<void()> onDone)
    {
        onClearDone = onDone;
        claimer.Claim([this]()
        {
            endAddress = 0;
            startAddress = 0;

            assert(sequencer.Finished());
            sequencer.Load([this]() {
                sequencer.Sequence(),
                [this]()
                {
                    flash.EraseAll([this]() { sequencer.Continue(); });
                },
                infra::Hold(),
                [this]()
                {
                    claimer.Release();
                    onClearDone();
                };
            });
        });
    }

    CyclicFlashStorage::Iterator CyclicFlashStorage::Begin() const
    {
        return Iterator(*this);
    }

    void CyclicFlashStorage::Recover()
    {
        recoverClaimer.Claim([this]()
        {
            endAddress = 0;
            startAddress = 0;

            assert(sequencer.Finished());
            sequencer.Load([this]() {
                sequencer.Sequence(),
                    infra::Hold(),  // Wait for the onFlashInitialized gate to be opened
                    infra::ForEach(0, flash.NumberOfSectors()),
                        infra::Nest(
                            [this](uint32_t sectorIndex)->infra::Sequencer& { return RecoverSector(sectorIndex); }
                        ),
                    infra::EndForEach(),
                    infra::If([this]() { return recoverPhase == RecoverPhase::corrupt || recoverPhase == RecoverPhase::searchingStartOrEmpty; }),
                        [this]()
                        {
                            startAddress = 0;
                            endAddress = startAddress;
                            flash.EraseAll([this]() { sequencer.Continue(); });
                        },
                        infra::Hold(),
                    infra::ElseIf([this]() { return endAddress != startAddress; }),
                        infra::Nest(
                            [this]()->infra::Sequencer& { return RecoverEndAddress(); }
                        ),
                    infra::EndIf(),
                    [this]()
                    {
                        recoverClaimer.Release();
                        recoverPhase = RecoverPhase::done;
                    };
            });

            onFlashInitialized.OnOpen([this]() { sequencer.Continue(); });
        });
    }

    infra::Sequencer& CyclicFlashStorage::RecoverSector(uint32_t sectorIndex)
    {
        nestedSequencer.Load([this, sectorIndex]()
        {
            nestedSequencer.Sequence(),
            [this, sectorIndex]()
            {
                flash.ReadBuffer(infra::MakeByteRange(sectorStatus), flash.AddressOfSector(sectorIndex), [this]() { sequencer.Continue(); });
            },
            infra::Hold(),
            [this, sectorIndex]()
            {
                if (sectorStatus == SectorStatus::firstInCycle)
                {
                    if (recoverPhase == RecoverPhase::searchingStartOrEmpty)
                    {
                        startAddress = flash.AddressOfSector(sectorIndex);
                        endAddress = startAddress;

                        if (sectorIndex == 0)
                            recoverPhase = RecoverPhase::checkingUsedFollowedByEmpty;
                        else
                            recoverPhase = RecoverPhase::checkingAllUsed;
                    }
                    else
                        recoverPhase = RecoverPhase::corrupt;
                }
                else if (sectorStatus == SectorStatus::used)
                {
                    if (recoverPhase == RecoverPhase::checkingAllUsedOrAllEmpty)
                        recoverPhase = RecoverPhase::checkingAllUsed;
                    else if (recoverPhase == RecoverPhase::checkingUsedFollowedByEmpty
                        || recoverPhase == RecoverPhase::searchingStartOrEmpty
                        || recoverPhase == RecoverPhase::checkingAllUsed)
                    {}
                    else
                        recoverPhase = RecoverPhase::corrupt;
                }
                else if (sectorStatus == SectorStatus::empty)
                {
                    if (recoverPhase == RecoverPhase::searchingStartOrEmpty)
                    {
                        startAddress = flash.StartOfNextSectorCyclical(flash.AddressOfSector(sectorIndex));
                        endAddress = flash.AddressOfSector(sectorIndex);

                        recoverPhase = RecoverPhase::checkingAllUsedOrAllEmpty;
                    }
                    else if (recoverPhase == RecoverPhase::checkingAllUsedOrAllEmpty)
                    {
                        startAddress = 0;
                        endAddress = 0;
                        recoverPhase = RecoverPhase::checkingAllEmpty;
                    }
                    else if (recoverPhase == RecoverPhase::checkingUsedFollowedByEmpty)
                    {
                        endAddress = flash.AddressOfSector(sectorIndex);
                        recoverPhase = RecoverPhase::checkingAllEmpty;
                    }
                    else if (recoverPhase == RecoverPhase::checkingAllEmpty)
                    {}
                    else
                        recoverPhase = RecoverPhase::corrupt;
                }
                else
                    recoverPhase = RecoverPhase::corrupt;
            };
        });

        return nestedSequencer;
    }

    infra::Sequencer& CyclicFlashStorage::RecoverEndAddress()
    {
        nestedSequencer.Load([this]()
        {
            nestedSequencer.Sequence(),
            [this]()
            {
                endAddress = flash.StartOfPreviousSectorCyclical(endAddress);
                ++endAddress;

                flash.ReadBuffer(infra::MakeByteRange(blockHeader), endAddress, [this]() { sequencer.Continue(); });
            },
            infra::Hold(),
            infra::While([this]() { return blockHeader.status != BlockStatus::empty && !flash.AtStartOfSector(endAddress); }),
                infra::If([this]() { return blockHeader.status == BlockStatus::dataReady; }),
                    [this]()
                    {
                        endAddress += sizeof(BlockHeader);
                    },
                    infra::If([this]() { return endAddress + blockHeader.BlockLength() <= flash.TotalSize() && flash.SectorOfAddress(endAddress) == flash.SectorOfAddress(endAddress + blockHeader.BlockLength()); }),
                        [this]()
                        {
                            endAddress += blockHeader.BlockLength();
                            flash.ReadBuffer(infra::MakeByteRange(blockHeader), endAddress, [this]() { sequencer.Continue(); });
                        },
                        infra::Hold(),
                    infra::Else(),
                        [this]()
                        {
                            endAddress = flash.StartOfNextSectorCyclical(endAddress);
                        },
                    infra::EndIf(),
                infra::ElseIf([this]() { return blockHeader.status == BlockStatus::writingLength; }),
                    [this]()
                    {
                        endAddress += sizeof(BlockHeader);
                        flash.ReadBuffer(infra::MakeByteRange(blockHeader), endAddress, [this]() { sequencer.Continue(); });
                    },
                    infra::Hold(),
                infra::ElseIf([this]() { return blockHeader.status == BlockStatus::writingData; }),
                    [this]()
                    {
                        endAddress += sizeof(BlockHeader) + blockHeader.BlockLength();
                    },
                    infra::If([this]() { return flash.AddressOffsetInSector(endAddress) <= flash.SizeOfSector(flash.SectorOfAddress(endAddress)); }),
                        [this]()
                        {
                            flash.ReadBuffer(infra::MakeByteRange(blockHeader), endAddress, [this]() { sequencer.Continue(); });
                        },
                        infra::Hold(),
                    infra::Else(),
                        [this]()
                        {
                            // The BlockLength() field is corrupt, skip to next sector
                            endAddress = flash.StartOfNextSectorCyclical(endAddress);
                        },
                    infra::EndIf(),
                infra::ElseIf([this]() { return blockHeader.status == BlockStatus::emptyUntilEnd; }),
                    [this]()
                    {
                        endAddress = flash.StartOfNextSectorCyclical(endAddress);
                    },
                infra::Else(),
                    [this]()
                    {
                        endAddress = flash.StartOfNextSectorCyclical(endAddress);
                    },
                infra::EndIf(),
            infra::EndWhile();
        });

        return nestedSequencer;
    }

    infra::Sequencer& CyclicFlashStorage::EraseSectorIfAtStart()
    {
        nestedSequencer.Load([this]()
        {
            nestedSequencer.Sequence(),
                infra::If([this]() { return flash.AddressOffsetInSector(endAddress) == 0; }),
                    [this]()
                    {
                        flash.EraseSector(flash.SectorOfAddress(endAddress), [this]() { sequencer.Continue(); });
                    },
                    infra::Hold(),
                    [this]()
                    {
                        flash.ReadBuffer(infra::MakeByteRange(sectorStatus), flash.StartOfNextSectorCyclical(endAddress), [this]() { sequencer.Continue(); });
                    },
                    infra::Hold(),
                    infra::If([this]() { return sectorStatus == SectorStatus::used; }),
                        [this]()
                        {
                            for (auto& iterator: iterators)
                                iterator.SectorIsErased(flash.SectorOfAddress(startAddress));

                            sectorStatus = SectorStatus::firstInCycle;
                            startAddress = flash.StartOfNextSectorCyclical(endAddress);
                            flash.WriteBuffer(infra::MakeByteRange(sectorStatus), startAddress, [this]() { sequencer.Continue(); });
                        },
                        infra::Hold(),
                    infra::EndIf(),
                infra::EndIf();
        });

        return nestedSequencer;
    }

    infra::Sequencer& CyclicFlashStorage::FillSectorIfDataDoesNotFit(std::size_t size)
    {
        nestedSequencer.Load([this, size]()
        {
            nestedSequencer.Sequence(),
                infra::If([this, size]() { return flash.AddressOffsetInSector(endAddress) + sizeof(BlockHeader) + size > flash.SizeOfSector(flash.SectorOfAddress(endAddress)); }),
                    [this]()
                    {
                        blockHeader.status = BlockStatus::emptyUntilEnd;
                        flash.WriteBuffer(infra::MakeByteRange(blockHeader.status), endAddress, [this]() { sequencer.Continue(); });
                        endAddress = flash.StartOfNextSectorCyclical(endAddress);
                    },
                    infra::Hold(),
                infra::EndIf();
        });

        return nestedSequencer;
    }

    infra::Sequencer& CyclicFlashStorage::WriteSectorStatusIfAtStartOfSector()
    {
        nestedSequencer.Load([this]()
        {
            nestedSequencer.Sequence(),
                infra::If([this]() { return flash.AtStartOfSector(endAddress); }),
                    [this]()
                    {
                        flash.ReadBuffer(infra::MakeByteRange(sectorStatus), flash.StartOfNextSectorCyclical(endAddress), [this]() { sequencer.Continue(); });
                    },
                    infra::Hold(),
                    infra::If([this]() { return sectorStatus == SectorStatus::used; }),
                        [this]()
                        {
                            sectorStatus = SectorStatus::firstInCycle;
                            flash.WriteBuffer(infra::MakeByteRange(sectorStatus), flash.StartOfNextSectorCyclical(endAddress), [this]() { sequencer.Continue(); });
                        },
                        infra::Hold(),
                    infra::EndIf(),
                    [this]()
                    {
                        sectorStatus = endAddress == startAddress ? SectorStatus::firstInCycle : SectorStatus::used;
                        flash.WriteBuffer(infra::MakeByteRange(sectorStatus), endAddress, [this]() { sequencer.Continue(); });
                        ++endAddress;
                        if (endAddress == flash.TotalSize())
                            endAddress = 0;
                    },
                    infra::Hold(),
                infra::EndIf();
        });

        return nestedSequencer;
    }

    infra::Sequencer& CyclicFlashStorage::WriteRange(infra::ConstByteRange range)
    {
        nestedSequencer.Load([this, range]()
        {
            nestedSequencer.Sequence(),
                [this, range]()
                {
                    assert(sizeof(BlockHeader) + range.size() <= flash.SizeOfSector(flash.SectorOfAddress(endAddress)));
                    blockHeader.status = BlockStatus::writingLength;
                    flash.WriteBuffer(infra::MakeByteRange(blockHeader.status), endAddress, [this]() { sequencer.Continue(); });
                },
                infra::Hold(),
                [this, range]()
                {
                    blockHeader.SetBlockLength(range.size());
                    flash.WriteBuffer(infra::MakeByteRange(blockHeader.BlockLength()), endAddress + 1, [this]() { sequencer.Continue(); });
                },
                infra::Hold(),
                [this]()
                {
                    blockHeader.status = BlockStatus::writingData;
                    flash.WriteBuffer(infra::MakeByteRange(blockHeader.status), endAddress, [this]() { sequencer.Continue(); });
                },
                infra::Hold(),
                [this, range]()
                {
                    flash.WriteBuffer(range, endAddress + 3, [this]() { sequencer.Continue(); });
                },
                infra::Hold(),
                [this, range]()
                {
                    blockHeader.status = BlockStatus::dataReady;
                    flash.WriteBuffer(infra::MakeByteRange(blockHeader.status), endAddress, [this]() { sequencer.Continue(); });
                    endAddress += range.size() + 3;
                    if (endAddress == flash.TotalSize())
                        endAddress = 0;
                },
                infra::Hold();
        });

        return nestedSequencer;
    }

    CyclicFlashStorage::Iterator::Iterator(const CyclicFlashStorage& store)
        : store(store)
        , loadStartAddressDelayed(true)
        , address(0)
        , claimer(store.resource)
    {
        store.iterators.push_front(*this);
    }

    CyclicFlashStorage::Iterator::Iterator(const Iterator& other)
        : store(other.store)
        , loadStartAddressDelayed(other.loadStartAddressDelayed)
        , address(other.address)
        , claimer(store.resource)
    {
        store.iterators.push_front(*this);
    }

    CyclicFlashStorage::Iterator::~Iterator()
    {
        store.iterators.erase_slow(*this);
    }

    CyclicFlashStorage::Iterator& CyclicFlashStorage::Iterator::operator=(const Iterator& other)
    {
        assert(&store == &other.store);

        loadStartAddressDelayed = other.loadStartAddressDelayed;
        address = other.address;

        return *this;
    }

    void CyclicFlashStorage::Iterator::Read(infra::ByteRange buffer, infra::Function<void(infra::ByteRange result)> onDone)
    {
        readBuffer = buffer;
        found = false;

        claimer.Claim([this, onDone]()
        {
            if (loadStartAddressDelayed)
            {
                loadStartAddressDelayed = false;
                address = store.startAddress;
            }

            assert(sequencer.Finished());
            sequencer.Load([this, onDone]() {
                sequencer.Sequence(),
                infra::Nest(
                    [this]()->infra::Sequencer& { return ReadSectorStatusIfAtStart(); }
                ),
                infra::While([this]() { return sectorStatus == SectorStatus::used && !found; }),
                    [this]()
                    {
                        store.flash.ReadBuffer(infra::MakeByteRange(blockHeader), address, [this]() { sequencer.Continue(); });
                        if (store.flash.SectorOfAddress(address + sizeof(blockHeader)) == store.flash.SectorOfAddress(address))
                            address += sizeof(blockHeader);
                        else
                            address = store.flash.StartOfNextSectorCyclical(address);
                    },
                    infra::Hold(),
                    infra::If([this]() { return blockHeader.status == BlockStatus::dataReady; }),
                        infra::If([this]() { return store.flash.AddressOffsetInSector(address) + blockHeader.BlockLength() <= store.flash.SizeOfSector(store.flash.SectorOfAddress(address)); }),
                            [this]()
                            {
                                readBuffer.shrink_from_back_to(blockHeader.BlockLength());
                                store.flash.ReadBuffer(readBuffer, address, [this]() { sequencer.Continue(); });
                                address += blockHeader.BlockLength();
                                if (address == store.flash.TotalSize())
                                    address = 0;
                                found = true;
                            },
                            infra::Hold(),
                        infra::Else(),
                            [this]()
                            {
                                address = store.flash.StartOfNextSectorCyclical(address);
                            },
                        infra::EndIf(),
                    infra::ElseIf([this]() { return blockHeader.status == BlockStatus::emptyUntilEnd || blockHeader.status == BlockStatus::empty; }),
                        [this]()
                        {
                            if (!store.flash.AtStartOfSector(address))
                                address = store.flash.StartOfNextSectorCyclical(address);
                        },
                    infra::ElseIf([this]() { return blockHeader.status == BlockStatus::writingLength; }),
                        // Do nothing
                    infra::ElseIf([this]() { return blockHeader.status == BlockStatus::writingData; }),
                        [this]()
                        {
                            address += blockHeader.BlockLength();
                        },
                    infra::Else(),
                        [this]()
                        {
                            address = store.flash.StartOfNextSectorCyclical(address);
                        },
                    infra::EndIf(),
                    infra::Nest(
                        [this]()->infra::Sequencer& { return ReadSectorStatusIfAtStart(); }
                    ),
                infra::EndWhile(),
                [this, onDone]()
                {
                    if (!found)
                        readBuffer.clear();
                    onDone(readBuffer);
                },
                [this]()
                {
                    claimer.Release();
                };
            });
        });
    }

    void CyclicFlashStorage::Iterator::SectorIsErased(uint32_t sectorIndex)
    {
        if (store.flash.SectorOfAddress(address) == sectorIndex)
            address = store.flash.StartOfNextSectorCyclical(address);
    }

    infra::Sequencer& CyclicFlashStorage::Iterator::ReadSectorStatusIfAtStart()
    {
        nestedSequencer.Load([this]()
        {
            nestedSequencer.Sequence(),
                infra::If([this]() { return store.flash.AtStartOfSector(address); }),
                    [this]()
                    {
                        store.flash.ReadBuffer(infra::MakeByteRange(sectorStatus), address, [this]() { sequencer.Continue(); });
                        address += sizeof(sectorStatus);
                    },
                    infra::Hold(),
                    [this]()
                    {
                        if (sectorStatus == SectorStatus::firstInCycle)
                            if (firstSectorToRead)
                                sectorStatus = SectorStatus::used;
                            else
                                sectorStatus = SectorStatus::empty;
                        firstSectorToRead = false;
                    },
                infra::EndIf();
        });

        return nestedSequencer;
    }
}
*/