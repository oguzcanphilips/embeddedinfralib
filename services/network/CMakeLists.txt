ccola_component(network static_library)

    ccola_sources(
        Address.cpp
        Address.hpp
        CertificatesMbedTls.cpp
        CertificatesMbedTls.hpp
        Connection.cpp
        Connection.hpp
        ConnectionMbedTls.cpp
        ConnectionMbedTls.hpp
        Datagram.hpp
        LlmnrResponder.cpp
        LlmnrResponder.hpp
        Mqtt.hpp
        MqttClientImpl.hpp
        MqttClientImpl.cpp
        Multicast.hpp
        TracingConnectionMbedTls.cpp
        TracingConnectionMbedTls.hpp
    )

    ccola_dependencies(
        infra.event
        infra.stream
        infra.syntax
        infra.timer
        crypto.mbed_tls
        hal.synchronous_interfaces
        services.tracer
    )

ccola_end_component()

ccola_subdirectories(
    test_doubles
    test
)
