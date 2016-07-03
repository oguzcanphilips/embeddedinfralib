#include "hal/interfaces/public/Spi.hpp"

namespace hal
{
    void SpiMaster::SendData(infra::ConstByteRange data, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion)
    {
        SendAndReceive(data, infra::ByteRange(), nextAction, actionOnCompletion);
    }

    void SpiMaster::ReceiveData(infra::ByteRange data, SpiAction nextAction, const infra::Function<void()>& actionOnCompletion)
    {
        SendAndReceive(infra::ConstByteRange(), data, nextAction, actionOnCompletion);
    }
}
