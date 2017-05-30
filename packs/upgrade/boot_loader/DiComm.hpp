#ifndef UPGRADE_DI_COMM_HPP
#define UPGRADE_DI_COMM_HPP

#include "infra/util/BoundedString.hpp"

namespace application
{
    class DiComm
    {
    public:
        DiComm() = default;
        DiComm(const DiComm& other) = delete;
        DiComm& operator=(const DiComm& other) = delete;

        virtual bool Initialize() = 0;
        virtual bool GetProps(infra::BoundedConstString port, infra::BoundedString& values) = 0;
        bool PutProps(infra::BoundedConstString port, infra::BoundedConstString values)
        {
            infra::BoundedString::WithStorage<0> responseDummy;
            return PutProps(port, values, responseDummy);
        }
        virtual bool PutProps(infra::BoundedConstString port, infra::BoundedConstString values, infra::BoundedString& result) = 0;

    protected:
        ~DiComm() = default;
    };
}

#endif
