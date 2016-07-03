#include "hal/synchronous_interfaces/public/SynchronousSpi.hpp"

namespace hal
{
    void SynchronousSpi::SendData(infra::ConstByteRange data, Action nextAction)
    {
        SendAndReceive(data, infra::ByteRange(), nextAction);
    }

    void SynchronousSpi::ReceiveData(infra::ByteRange data, Action nextAction)
    {
        SendAndReceive(infra::ConstByteRange(), data, nextAction);
    }
}
