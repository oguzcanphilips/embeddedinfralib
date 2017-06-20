#ifndef upgrade_pack_builder_test_IMAGE_SECURITY_NONE_HPP
#define upgrade_pack_builder_test_IMAGE_SECURITY_NONE_HPP

#include "packs/upgrade/pack/UpgradePackHeader.hpp"
#include "packs/upgrade/pack_builder/ImageSecurity.hpp"

namespace application
{
    class ImageSecurityNone
        : public application::ImageSecurity
    {
    public:
        virtual uint32_t EncryptionAndMacMethod() const override;
        virtual std::vector<uint8_t> Secure(const std::vector<uint8_t>& data) const override;
    };

    struct ImageHeaderNoSecurity
    {
        ImageHeaderPrologue header;
        uint32_t destinationAddress;
        uint32_t binaryLength;
    };
}

#endif
