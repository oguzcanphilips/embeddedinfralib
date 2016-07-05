#ifndef UPGRADE_DECRYPTOR_HPP
#define UPGRADE_DECRYPTOR_HPP

#include "infra/util/public/ByteRange.hpp"

namespace application
{
    class Decryptor
    {
    public:
        virtual infra::ByteRange StateBuffer() = 0;
        virtual void Reset() = 0;
        virtual void DecryptPart(infra::ByteRange data) = 0;
        virtual bool DecryptAndAuthenticate(infra::ByteRange data) = 0;

    protected:
        ~Decryptor() = default;
    };
}

#endif
