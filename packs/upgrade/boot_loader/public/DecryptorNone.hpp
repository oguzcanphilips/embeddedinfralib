#ifndef UPGRADE_DECRYPTOR_NONE_HPP
#define UPGRADE_DECRYPTOR_NONE_HPP

#include "packs/upgrade/boot_loader/public/Decryptor.hpp"

namespace application
{
    class DecryptorNone
        : public Decryptor
    {
    public:
        virtual infra::ByteRange StateBuffer() override;
        virtual void Reset() override;
        virtual void DecryptPart(infra::ByteRange data) override;
        virtual bool DecryptAndAuthenticate(infra::ByteRange data) override;
    };
}

#endif
