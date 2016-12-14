#ifndef UPGRADE_PACK_BUILDER_LIBRARY_INPUT_HEX_HPP
#define UPGRADE_PACK_BUILDER_LIBRARY_INPUT_HEX_HPP

#include "packs/upgrade/pack_builder/public/BinaryObject.hpp"
#include "hal/interfaces/public/FileSystem.hpp"
#include "packs/upgrade/pack_builder/public/ImageSecurity.hpp"
#include "packs/upgrade/pack_builder/public/Input.hpp"

namespace application
{
    class InputHex
        : public Input
    {
    public:
        InputHex(const std::string& targetName, const std::string& fileName, hal::FileSystem& fileSystem, const ImageSecurity& imageSecurity);

        virtual std::vector<uint8_t> Image() const override;

    private:
        std::pair<std::vector<uint8_t>, uint32_t> Linearize(const application::SparseVector<uint8_t>& data) const;

    private:
        const ImageSecurity& imageSecurity;
        application::BinaryObject contents;
    };
}

#endif
