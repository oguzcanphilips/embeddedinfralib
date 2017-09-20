#include "infra/stream/ByteOutputStream.hpp"
#include "packs/upgrade/boot_loader/ImageUpgraderDiComm.hpp"
#include "packs/upgrade/pack/UpgradePackHeader.hpp"
#include "infra/syntax/Json.hpp"

namespace application
{
    const uint32_t ImageUpgraderDiComm::chunkSizeDefault;
    const uint32_t ImageUpgraderDiComm::chunkSizeMax;

    ImageUpgraderDiComm::ImageUpgraderDiComm(const char* targetName, Decryptor& decryptor, DiComm& diComm)
        : ImageUpgrader(targetName, decryptor)
        , diComm(diComm)
    {}

    uint32_t ImageUpgraderDiComm::Upgrade(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t destinationAddress)
    {
        if (InitializeProtocol()
            && InitializeProperties()
            && PrepareDownload(imageSize)
            && SendFirmware(flash, imageAddress, imageSize)
            && WaitForState("idle"))
            return 0;
        else
            return upgradeErrorCodeExternalImageUpgradeFailed;
    }

    bool ImageUpgraderDiComm::InitializeProtocol()
    {
        for (int i = 0; i != 3; ++i)
            if (diComm.Initialize())
                return true;

        return false;
    }

    bool ImageUpgraderDiComm::InitializeProperties()
    {
        if (!diComm.PutProps("firmware", R"({"state":"idle"})"))
            return false;

        infra::BoundedString::WithStorage<256> firmwarePropertiesString;
        if (!diComm.GetProps("firmware", firmwarePropertiesString))
            return false;

        infra::JsonObject firmwareProperties(firmwarePropertiesString);
        infra::BoundedConstString state = firmwareProperties.GetString("state");
        bool canUpgrade = firmwareProperties.GetOptionalBoolean("canupgrade").ValueOr(true);
        maxChunkSize = firmwareProperties.GetOptionalInteger("maxchunksize").ValueOr(chunkSizeMax);

        if (firmwareProperties.Error() || state != "idle" || !canUpgrade)
            return false;

        return true;
    }

    bool ImageUpgraderDiComm::PrepareDownload(uint32_t imageSize)
    {
        infra::ByteOutputStream::WithStorage<64> properties;
        properties << infra::text << R"({"mandatory":true,"state":"downloading","size":)" << imageSize << R"(})";

        if (!diComm.PutProps("firmware", infra::BoundedConstString(reinterpret_cast<char*>(properties.Writer().Processed().begin()), properties.Writer().Processed().size())))
            return false;

        if (!WaitForState("downloading"))
            return false;

        return true;
    }

    bool ImageUpgraderDiComm::SendFirmware(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize)
    {
        FirmwareWriter writer(ImageDecryptor(), diComm, flash, imageAddress, imageSize, maxChunkSize);

        return writer.SendFirmware();
    }

    bool ImageUpgraderDiComm::WaitForState(infra::BoundedConstString expectedState)
    {
        bool ready;

        do
        {
            infra::BoundedString::WithStorage<256> firmwarePropertiesString;
            infra::JsonObject firmwareProperties(firmwarePropertiesString);

            if (!diComm.GetProps("firmware", firmwarePropertiesString))
                return false;

            firmwareProperties = infra::JsonObject(firmwarePropertiesString);
            infra::BoundedConstString state = firmwareProperties.GetString("state");
            if (state == "error")
                return false;

            ready = state == expectedState;
        } while (!ready);

        return true;
    }

    bool ImageUpgraderDiComm::ResetState()
    {
        if (!diComm.PutProps("firmware", R"({"state":"idle"})"))
            return false;

        return true;
    }

    ImageUpgraderDiComm::FirmwareWriter::FirmwareWriter(Decryptor& decryptor, DiComm& diComm, hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t maxChunkSize)
        : decryptor(decryptor)
        , diComm(diComm)
        , flash(flash)
        , imageAddress(imageAddress)
        , imageSize(imageSize)
        , maxChunkSize(maxChunkSize)
    {}

    bool ImageUpgraderDiComm::FirmwareWriter::SendFirmware()
    {
        while (imageSizeSent != imageSize)
        {
            ReadChunkFromFlash();
            
            if (!SendChunk())
                return false;
        }

        return true;
    }

    void ImageUpgraderDiComm::FirmwareWriter::ReadChunkFromFlash()
    {
        chunk.resize(std::min<uint32_t>(std::min(imageSize - imageSizeSent, maxChunkSize), chunk.max_size()));
        flash.ReadBuffer(infra::MakeRange(chunk), imageAddress + imageSizeSent);
        decryptor.DecryptPart(infra::MakeRange(chunk));
        ConvertChunkToBase64();
    }

    bool ImageUpgraderDiComm::FirmwareWriter::SendChunk()
    {
        infra::ByteOutputStream::WithStorage<base64ChunkSizeMax> chunkData;
        chunkData << infra::text << R"({"data":")" << infra::data << infra::MemoryRange<uint8_t>(base64EncodedChunk.begin(), base64EncodedChunk.end()) << infra::text << R"("})";
        infra::BoundedString::WithStorage<128> result;
        if (!diComm.PutProps("firmware", infra::BoundedConstString(reinterpret_cast<char*>(chunkData.Writer().Processed().begin()), chunkData.Writer().Processed().size()), result))
            return false;

        infra::JsonObject resultProperties = infra::JsonObject(result);
        if (resultProperties.GetString("state") == "error")
            return false;

        imageSizeSent += chunk.size();
        imageSizeSentEncoded += base64EncodedChunk.size();
        
        return true;
    }

    void ImageUpgraderDiComm::FirmwareWriter::ConvertChunkToBase64()
    {
        static const char* encodeTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        base64EncodedChunk.clear();

        uint8_t bitIndex = 2;                                                                                           //TICS !OLC#006
        uint8_t encodedByte = 0;

        for (uint8_t chunkByte : chunk)
        {
            encodedByte |= chunkByte >> bitIndex;
            base64EncodedChunk.push_back(encodeTable[encodedByte]);

            encodedByte = static_cast<uint8_t>(chunkByte << (8 - bitIndex)) >> 2;                                       //TICS !POR#006

            bitIndex += 2;

            if (bitIndex == 8)
            {
                base64EncodedChunk.push_back(encodeTable[encodedByte]);
                encodedByte = 0;
                bitIndex = 2;
            }
        }

        if ((base64EncodedChunk.size() & 3) != 0)
            base64EncodedChunk.push_back(encodeTable[encodedByte]);
        if ((base64EncodedChunk.size() & 3) != 0)
            base64EncodedChunk.push_back('=');
        if ((base64EncodedChunk.size() & 3) != 0)
            base64EncodedChunk.push_back('=');
    }
}
