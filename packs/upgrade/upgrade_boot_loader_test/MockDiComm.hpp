#ifndef UPGRADE_MOCK_DI_COMM_HPP
#define UPGRADE_MOCK_DI_COMM_HPP

#include "gmock/gmock.h"
#include "upgrade_boot_loader/DiComm.hpp"
#include <string>
#include <utility>

namespace application
{
    class MockDiComm
        : public DiComm
    {
    public:
        virtual bool Initialize() override;
        virtual bool GetProps(infra::BoundedConstString port, infra::BoundedString& values) override;
        virtual bool PutProps(infra::BoundedConstString port, infra::BoundedConstString values, infra::BoundedString& result) override;

        using GetPropsResult = std::pair<bool, std::string>;
        using PutPropsResult = std::pair<bool, std::string>;

        MOCK_METHOD0(InitializeMock, bool());
        MOCK_METHOD1(GetPropsMock, GetPropsResult(std::string port));
        MOCK_METHOD2(PutPropsMock, PutPropsResult(std::string port, std::string values));
    };
}

#endif
