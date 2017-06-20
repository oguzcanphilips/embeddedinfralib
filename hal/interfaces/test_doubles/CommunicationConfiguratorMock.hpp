#ifndef HAL_COMMUNICATION_CONFIGURATOR_MOCK_HPP
#define HAL_COMMUNICATION_CONFIGURATOR_MOCK_HPP

#include "gmock/gmock.h"
#include "hal/interfaces/CommunicationConfigurator.hpp"

namespace hal
{
    //TICS -INT#002: A mock or stub may have public data
    class CommunicationConfiguratorMock
        : public CommunicationConfigurator
    {
    public:
        MOCK_METHOD0(ActivateConfiguration, void());
        MOCK_METHOD0(DeactivateConfiguration, void());
    };
}

#endif
