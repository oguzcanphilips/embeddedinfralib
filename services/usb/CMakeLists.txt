ccola_component(usb static_library)

    ccola_sources(
        UsbDevice.cpp
        UsbEndPoint.cpp
        UsbInterfaceCustomHid.cpp
        UsbInterfaceVirtualComPort.cpp
        UsbDevice.hpp
        UsbEndPoint.hpp
        UsbInterface.hpp
        UsbInterfaceCustomHid.hpp
        UsbInterfaceVirtualComPort.hpp
    )

    ccola_dependencies(
        hal.interfaces
        infra.stream
    )

ccola_end_component()
