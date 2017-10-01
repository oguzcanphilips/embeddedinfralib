#ifndef PROTOBUF_ECHO_HPP
#define PROTOBUF_ECHO_HPP

#include "infra/util/Function.hpp"
#include "protobuf/echo/ProtoParser.hpp"
#include "services/network/Connection.hpp"

namespace services
{
    class Echo;

    class Service
        : public infra::Observer<Service, Echo>
    {
    public:
        Service(Echo& echo, uint32_t id);

        virtual void Handle(uint32_t methodId, services::ProtoParser& parser) = 0;

        Echo& Rpc();
        uint32_t ServiceId() const;

    private:
        uint32_t serviceId;
    };

    class ServiceProxy
        : public infra::IntrusiveList<ServiceProxy>::NodeType
    {
    public:
        ServiceProxy(Echo& echo, uint32_t id);

        Echo& Rpc();
        void RequestSend(infra::Function<void()> onGranted);
        void GrantSend();

    private:
        Echo& echo;
        uint32_t serviceId;
        infra::Function<void()> onGranted;
    };

    class Echo
        : public infra::Subject<Service>
        , public services::ConnectionObserver
    {
    public:
        Echo(services::Connection& connection);

        virtual void SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream) override;
        virtual void DataReceived() override;

        void RequestSend(ServiceProxy& serviceProxy);
        infra::DataOutputStream SendStream();
        void Send();

    private:
        void ExecuteMethod(uint32_t serviceId, uint32_t methodId, services::ProtoParser& argument);

    private:
        infra::SharedPtr<infra::DataOutputStream> sendStream;
        infra::IntrusiveList<ServiceProxy> sendRequesters;

        static const uint32_t maxMessageSize = 1024;
    };
}

#endif
