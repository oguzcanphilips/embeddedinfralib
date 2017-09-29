#include "protobuf/protobuf_cpp_infra/Echo.hpp"
#include "protobuf/protobuf_cpp_infra/ProtoFormatter.hpp"

namespace services
{
    Service::Service(Echo& echo, uint32_t id)
        : infra::Observer<Service, Echo>(echo)
        , serviceId(id)
    {}

    Echo& Service::Rpc()
    {
        return Subject();
    }

    uint32_t Service::ServiceId() const
    {
        return serviceId;
    }

    ServiceProxy::ServiceProxy(Echo& echo, uint32_t id)
        : echo(echo)
        , serviceId(id)
    {}

    Echo& ServiceProxy::Rpc()
    {
        return echo;
    }

    void ServiceProxy::RequestSend(infra::Function<void()> onGranted)
    {
        this->onGranted = onGranted;
        echo.RequestSend(*this);
    }

    void ServiceProxy::GrantSend()
    {
        onGranted();
    }

    void Echo::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        sendStream = stream;

        ServiceProxy& proxy = sendRequesters.front();
        sendRequesters.pop_front();
        proxy.GrantSend();
    }

    void Echo::DataReceived()
    {
        infra::SharedPtr<infra::DataInputStream> stream = services::ConnectionObserver::Subject().ReceiveStream();
        services::ProtoParser parser(*stream);
        uint32_t serviceId = static_cast<uint32_t>(parser.GetVarInt());
        services::ProtoParser::Field message = parser.GetField();
        if (!stream->Failed())
        {
            assert(message.first.Is<services::ProtoLengthDelimited>());
            services::ProtoParser argument(message.first.Get<services::ProtoLengthDelimited>().Parser());
            uint32_t methodId = message.second;
            ExecuteMethod(serviceId, methodId, argument);
            services::ConnectionObserver::Subject().AckReceived();
        }
    }

    void Echo::RequestSend(ServiceProxy& serviceProxy)
    {
        if (sendRequesters.empty() && !sendStream)
            services::ConnectionObserver::Subject().RequestSendStream(maxMessageSize);

        sendRequesters.push_back(serviceProxy);
    }

    infra::DataOutputStream Echo::SendStream()
    {
        return *sendStream;
    }

    void Echo::Send()
    {
        sendStream = nullptr;

        if (!sendRequesters.empty())
            services::ConnectionObserver::Subject().RequestSendStream(maxMessageSize);
    }

    void Echo::ExecuteMethod(uint32_t serviceId, uint32_t methodId, services::ProtoParser& argument)
    {
        infra::Subject<Service>::NotifyObservers([serviceId, methodId, &argument](Service& service)
        {
            if (service.ServiceId() == serviceId)
                service.Handle(methodId, argument);
        });
    }
}
