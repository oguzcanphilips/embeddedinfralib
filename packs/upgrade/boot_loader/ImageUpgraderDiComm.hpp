#ifndef UPGRADE_IMAGE_UPGRADER_DI_COMM_HPP
#define UPGRADE_IMAGE_UPGRADER_DI_COMM_HPP

#include "infra/util/BoundedVector.hpp"
#include "packs/upgrade/boot_loader/Decryptor.hpp"
#include "packs/upgrade/boot_loader/DiComm.hpp"
#include "packs/upgrade/boot_loader/ImageUpgrader.hpp"

namespace application
{
    class ImageUpgraderDiComm
        : public ImageUpgrader
    {
    public:
        template<std::size_t Size>
            using WithStorage = infra::WithStorage<ImageUpgraderDiComm, std::array<uint8_t, Size>>;

        ImageUpgraderDiComm(infra::ByteRange buffer, const char* targetName, Decryptor& decryptor, DiComm& diComm);

        virtual uint32_t Upgrade(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t destinationAddress) override;

    private:
        bool InitializeProtocol();
        bool InitializeProperties();
        bool PrepareDownload(uint32_t imageSize);
        bool SendFirmware(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize);
        bool WaitForIdle();
        bool ResetState();

    private:
        DiComm& diComm;
        infra::ByteRange buffer;
        uint32_t maxChunkSize = 0;

        static const uint32_t chunkSizeDefault = 128;
        static const uint32_t chunkSizeMax = 1024;

        class FirmwareWriter
        {
        public:
            FirmwareWriter(Decryptor& decryptor, DiComm& diComm, hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t maxChunkSize, infra::ByteRange buffer);

            bool SendFirmware();

        private:
            void ReadChunkFromFlash();
            bool SendChunk();
            void ConvertChunkToBase64();

        private:
            Decryptor& decryptor;
            DiComm& diComm;
            hal::SynchronousFlash& flash;
            uint32_t imageAddress;
            uint32_t imageSize;
            uint32_t maxChunkSize;
            infra::ByteRange buffer;
            uint32_t imageSizeSent = 0;
            uint32_t imageSizeSentEncoded = 0;

            infra::BoundedVector<uint8_t>::WithMaxSize<chunkSizeMax> chunk;
            infra::BoundedVector<uint8_t>::WithMaxSize<(chunkSizeMax + 2) / 3 * 4> base64EncodedChunk;
        };
    };
}

#endif