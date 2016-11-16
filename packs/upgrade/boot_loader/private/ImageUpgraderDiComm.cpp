#include "infra/stream/public/ByteOutputStream.hpp"
#include "packs/upgrade/boot_loader/public/ImageUpgraderDiComm.hpp"
#include "infra/json/public/Json.hpp"

namespace application
{
    ImageUpgraderDiComm::ImageUpgraderDiComm(infra::ByteRange buffer, const char* targetName, Decryptor& decryptor, DiComm& diComm)
        : ImageUpgrader(targetName, decryptor)
        , diComm(diComm)
        , buffer(buffer)
    {}

    bool ImageUpgraderDiComm::Upgrade(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t destinationAddress)
    {
        return InitializeProtocol()
            && InitializeProperties()
            && PrepareDownload(imageSize)
            && SendFirmware(flash, imageAddress, imageSize)
            && WaitForIdle();
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

        infra::BoundedString firmwarePropertiesString(infra::ReinterpretCastMemoryRange<char>(buffer));
        if (!diComm.GetProps("firmware", firmwarePropertiesString))
            return false;

        infra::JsonObject firmwareProperties(firmwarePropertiesString);
        infra::BoundedConstString state = firmwareProperties.GetString("state");
        bool canUpgrade = firmwareProperties.GetOptionalBoolean("canupgrade").ValueOr(true);
        maxChunkSize = firmwareProperties.GetOptionalInteger("maxchunksize").ValueOr((chunkSizeMax + 2) / 3 * 4);

        if (firmwareProperties.Error() || state != "idle" || !canUpgrade)
            return false;

        return true;
    }

    bool ImageUpgraderDiComm::PrepareDownload(uint32_t imageSize)
    {
        infra::ByteOutputStream properties(buffer);
        properties << infra::text << R"({"mandatory":true,"state":"downloading","size":)" << imageSize << R"(})";

        if (!diComm.PutProps("firmware", infra::BoundedConstString(reinterpret_cast<char*>(properties.Processed().begin()), properties.Processed().size())))
            return false;

        return true;
    }

    bool ImageUpgraderDiComm::SendFirmware(hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize)
    {
        FirmwareWriter writer(ImageDecryptor(), diComm, flash, imageAddress, imageSize, maxChunkSize, buffer);

        return writer.SendFirmware();
    }

    bool ImageUpgraderDiComm::WaitForIdle()
    {
        bool ready;

        do
        {
            infra::BoundedString firmwarePropertiesString(infra::ReinterpretCastMemoryRange<char>(buffer));
            infra::JsonObject firmwareProperties(firmwarePropertiesString);

            if (!diComm.GetProps("firmware", firmwarePropertiesString))
                return false;

            firmwareProperties = infra::JsonObject(firmwarePropertiesString);
            infra::BoundedConstString state = firmwareProperties.GetString("state");
            if (state == "error")
                return false;

            ready = state == "idle";
        } while (!ready);

        return true;
    }

    bool ImageUpgraderDiComm::ResetState()
    {
        if (!diComm.PutProps("firmware", R"({"state":"idle"})"))
            return false;

        return true;
    }

    ImageUpgraderDiComm::FirmwareWriter::FirmwareWriter(Decryptor& decryptor, DiComm& diComm, hal::SynchronousFlash& flash, uint32_t imageAddress, uint32_t imageSize, uint32_t maxChunkSize, infra::ByteRange buffer)
        : decryptor(decryptor)
        , diComm(diComm)
        , flash(flash)
        , imageAddress(imageAddress)
        , imageSize(imageSize)
        , maxChunkSize(maxChunkSize)
        , buffer(buffer)
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
        chunk.resize(std::min(imageSize - imageSizeSent, maxChunkSize / 4 * 3));
        flash.ReadBuffer(infra::MakeRangeFromContainer(chunk), imageAddress + imageSizeSent);
        decryptor.DecryptPart(infra::MakeRangeFromContainer(chunk));
        ConvertChunkToBase64();
    }

    bool ImageUpgraderDiComm::FirmwareWriter::SendChunk()
    {
        infra::ByteOutputStream chunkData(buffer);
        chunkData << infra::text << R"({"data":")" << infra::data << infra::MemoryRange<uint8_t>(base64EncodedChunk.begin(), base64EncodedChunk.end()) << infra::text << R"("})";
        infra::BoundedString::WithStorage<128> result;
        if (!diComm.PutProps("firmware", infra::BoundedConstString(reinterpret_cast<char*>(chunkData.Processed().begin()), chunkData.Processed().size()), result))
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

        uint8_t bitIndex = 2;
        uint8_t encodedByte = 0;

        for (uint8_t chunkByte : chunk)
        {
            encodedByte |= chunkByte >> bitIndex;
            base64EncodedChunk.push_back(encodeTable[encodedByte]);

            encodedByte = static_cast<uint8_t>(static_cast<uint8_t>(chunkByte << (8 - bitIndex)) >> 2);

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
