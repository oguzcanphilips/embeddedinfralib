#include "packs/upgrade/boot_loader/public/DecryptorNone.hpp"

namespace application
{
    infra::ByteRange DecryptorNone::StateBuffer()
    {
        return infra::ByteRange();
    }

    void DecryptorNone::Reset()
    {}

    void DecryptorNone::DecryptPart(infra::ByteRange data)
    {}

    bool DecryptorNone::DecryptAndAuthenticate(infra::ByteRange data)
    {
        return true;
    }
}
