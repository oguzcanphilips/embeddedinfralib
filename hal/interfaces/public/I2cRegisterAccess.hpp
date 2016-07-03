#ifndef HAL_I2C_REGISTER_ACCESS_HPP
#define HAL_I2C_REGISTER_ACCESS_HPP

#include "hal/interfaces/public/I2c.hpp"
#include "infra/util/public/WithStorage.hpp"

namespace hal
{
    class I2cMasterRegisterAccess
    {
    public:
        I2cMasterRegisterAccess(I2cMaster& i2cMaster, I2cAddress address);

        void ReadRegister(uint8_t dataRegister, infra::ByteRange data, const infra::Function<void()>& onDone);
        void WriteRegister(uint8_t dataRegister, infra::ConstByteRange data, const infra::Function<void()>& onDone);

    private:
        I2cMaster& i2cMaster;
        I2cAddress address;

        uint8_t dataRegister;
        infra::ByteRange readData;
        infra::ConstByteRange writeData;
        infra::Function<void()> onDone;
    };
}

#endif
