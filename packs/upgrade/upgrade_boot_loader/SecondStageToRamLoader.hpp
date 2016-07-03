#ifndef UPGRADE_SECOND_STAGE_TO_RAM_LOADER_HPP
#define UPGRADE_SECOND_STAGE_TO_RAM_LOADER_HPP

#include "hal/synchronous_interfaces/public/SynchronousFlash.hpp"
#include "upgrade_boot_loader/Decryptor.hpp"
#include "upgrade_boot_loader/Verifier.hpp"

namespace application
{
    class SecondStageToRamLoader
    {
    public:
        SecondStageToRamLoader(hal::SynchronousFlash& upgradePackFlash, const char* product);

        bool Load(infra::ByteRange ram, Decryptor& decryptor, const Verifier& verifier);
        void MarkAsError(uint32_t errorCode);

    private:
        bool TryLoadImage(infra::ByteRange ram, Decryptor& decryptor);

    private:
        hal::SynchronousFlash& upgradePackFlash;
        const char* product;
        uint32_t address = 0;
    };
}

#endif
