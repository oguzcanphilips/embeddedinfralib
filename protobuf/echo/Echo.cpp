#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "protobuf/echo/Echo.hpp"
#include "protobuf/echo/ProtoFormatter.hpp"

namespace services
{
    Service::Service(Echo& echo, uint32_t id)
        : echo(echo)
        , serviceId(id)
    {
        echo.AttachService(*this);
    }

    Service::~Service()
    {
        echo.DetachService(*this);
    }

    Echo& Service::Rpc()
    {
        return echo;
    }

    void Service::MethodDone()
    {
        inProgress = false;
        Rpc().ServiceDone(*this);
    }

    uint32_t Service::ServiceId() const
    {
        return serviceId;
    }

    ServiceProxy::ServiceProxy(Echo& echo, uint32_t id, uint32_t maxMessageSize)
        : echo(echo)
        , serviceId(id)
        , maxMessageSize(maxMessageSize)
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

    uint32_t ServiceProxy::MaxMessageSize() const
    {
        return maxMessageSize;
    }

    void Echo::SendStreamAvailable(infra::SharedPtr<infra::DataOutputStream>&& stream)
    {
        sendStream = std::move(stream);

        ServiceProxy& proxy = sendRequesters.front();
        sendRequesters.pop_front();
        proxy.GrantSend();
    }

    void Echo::DataReceived()
    {
        while (!serviceBusy)
        {
            infra::SharedPtr<infra::DataInputStream> stream = services::ConnectionObserver::Subject().ReceiveStream();
            services::ProtoParser parser(*stream);
            uint32_t serviceId = static_cast<uint32_t>(parser.GetVarInt());
            services::ProtoParser::Field message = parser.GetField();
            if (stream->Failed())
                break;

            assert(message.first.Is<services::ProtoLengthDelimited>());
            services::ProtoParser argument(message.first.Get<services::ProtoLengthDelimited>().Parser());
            uint32_t methodId = message.second;
            if (stream->Failed())
                break;

            ExecuteMethod(serviceId, methodId, argument);
            if (stream->Failed())
                std::abort();
            if (serviceBusy)
                break;

            services::ConnectionObserver::Subject().AckReceived();
        }
    }

    void Echo::RequestSend(ServiceProxy& serviceProxy)
    {
        if (sendRequesters.empty() && !sendStream)
            services::ConnectionObserver::Subject().RequestSendStream(serviceProxy.MaxMessageSize());

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
            services::ConnectionObserver::Subject().RequestSendStream(sendRequesters.front().MaxMessageSize());
    }

    void Echo::ServiceDone(Service& service)
    {
        if (serviceBusy && *serviceBusy == service.ServiceId())
        {
            serviceBusy = infra::none;
            infra::EventDispatcherWithWeakPtr::Instance().Schedule([](infra::SharedPtr<Echo> echo) { echo->DataReceived(); }, SharedFromThis());
        }
    }

    void Echo::AttachService(Service& service)
    {
        services.push_back(service);
    }

    void Echo::DetachService(Service& service)
    {
        services.erase(service);
    }

    void Echo::ExecuteMethod(uint32_t serviceId, uint32_t methodId, services::ProtoParser& argument)
    {
        for (auto& service : services)
        {
            if (service.ServiceId() == serviceId)
            {
                if (service.inProgress)
                    serviceBusy = serviceId;
                else
                {
                    service.inProgress = true;
                    service.Handle(methodId, argument);
                }
                break;
            }
        };
    }
}
