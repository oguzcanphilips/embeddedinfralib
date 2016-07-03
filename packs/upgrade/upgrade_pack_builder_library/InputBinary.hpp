#ifndef UPGRADE_PACK_BUILDER_LIBRARY_INPUT_BINARY_HPP
#define UPGRADE_PACK_BUILDER_LIBRARY_INPUT_BINARY_HPP

#include "hal/interfaces/public/FileSystem.hpp"
#include "upgrade_pack/UpgradePackHeader.hpp"
#include "upgrade_pack_builder_library/ImageSecurity.hpp"
#include "upgrade_pack_builder_library/Input.hpp"

namespace application
{
    class InputBinary
        : public Input
    {
    public:
        InputBinary(const std::string& targetName, const std::string& fileName, uint32_t destinationAddress,
            hal::FileSystem& fileSystem, const ImageSecurity& imageSecurity);
        InputBinary(const std::string& targetName, const std::vector<uint8_t>& contents, uint32_t destinationAddress,
            const ImageSecurity& imageSecurity);

        virtual std::vector<uint8_t> Image() const override;

    private:
        uint32_t destinationAddress;
        const ImageSecurity& imageSecurity;
        std::vector<uint8_t> image;
    };
}

#endif
