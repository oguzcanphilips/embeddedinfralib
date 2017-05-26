#ifndef UPGRADE_DI_COMM_UART_HPP
#define UPGRADE_DI_COMM_UART_HPP

#include "hal/synchronous_interfaces/SynchronousSerialCommunication.hpp"
#include "hal/synchronous_interfaces/TimeKeeper.hpp"
#include "packs/upgrade/boot_loader/DiComm.hpp"

namespace application
{
    class DiCommUart
        : public DiComm
    {
    public:
        DiCommUart(hal::SynchronousSerialCommunication& communication, hal::TimeKeeper& timeKeeper);
        
        virtual bool Initialize() override;
        virtual bool GetProps(infra::BoundedConstString port, infra::BoundedString& values) override;
        virtual bool PutProps(infra::BoundedConstString port, infra::BoundedConstString values, infra::BoundedString& result) override;

        using DiComm::PutProps;

    private:
        enum class Operation: uint8_t
        {
            initialize = 1,
            initializeResponse,
            putProps,
            execMethod = putProps,
            getProps,
            subscribe,
            unsubscribe,
            response,
            changeIndication,
            changeResponse,
            getProds,
            getPorts,
            addProps,
            delProps
        };

    private:
        bool ReceiveResponse(Operation responseOperation, infra::BoundedString& values);
        bool ReceiveData(infra::ByteRange data);

    private:
        hal::SynchronousSerialCommunication& communication;
        hal::TimeKeeper& timeKeeper;

        static const std::array<uint8_t, 2> startBytes;
    };
}

#endif
