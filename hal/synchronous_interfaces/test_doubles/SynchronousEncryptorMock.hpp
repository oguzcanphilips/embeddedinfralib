#ifndef SYNCHRONOUS_HAL_ENCRYPTOR_MOCK_HPP
#define SYNCHRONOUS_HAL_ENCRYPTOR_MOCK_HPP

#include "gmock/gmock.h"
#include "hal/synchronous_interfaces/SynchronousEncryptor.hpp"

namespace hal
{
    class SynchronousEncryptorMock
        : public SynchronousEncryptor
    {
    public:
        MOCK_METHOD0(StateBuffer, infra::ByteRange());
        MOCK_METHOD0(Reset, void());
        MOCK_METHOD1(Encrypt, void(infra::ByteRange data));
        MOCK_METHOD1(Decrypt, void(infra::ByteRange data));
    };

    class SynchronousEncryptorWithKeyGenerationMock
        : public SynchronousEncryptorWithKeyGeneration
    {
    public:
        MOCK_METHOD0(StateBuffer, infra::ByteRange());
        MOCK_METHOD0(Reset, void());
        MOCK_METHOD1(Encrypt, void(infra::ByteRange data));
        MOCK_METHOD1(Decrypt, void(infra::ByteRange data));
        MOCK_METHOD0(Key, infra::ByteRange());
        MOCK_METHOD0(GenerateKey, void());
    };
}

#endif
