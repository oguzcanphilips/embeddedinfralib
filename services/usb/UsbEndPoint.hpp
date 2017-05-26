#ifndef SERVICES_USB_END_POINT_HPP
#define SERVICES_USB_END_POINT_HPP

#include "services/usb/UsbDevice.hpp"
#include <cstdint>

namespace services
{
    class UsbEndPoint
    {
    public:
        UsbEndPoint(UsbDevice& device, uint8_t endPointNumber, hal::UsbEndPointType type, uint16_t maxPacketSize);
        UsbEndPoint(const UsbEndPoint& other) = delete;
        UsbEndPoint& operator=(const UsbEndPoint& other) = delete;

        UsbDevice& Device();
        uint8_t EndPointNumber() const;

    protected:
        ~UsbEndPoint();

    private:
        UsbDevice& device;
        uint8_t endPointNumber;
    };

    class UsbEndPointOut
        : public UsbEndPoint
    {
    public:
        using UsbEndPoint::UsbEndPoint;

        virtual void DataOut() = 0;
    };

    class UsbEndPointIn
        : public UsbEndPoint
    {
    public:
        using UsbEndPoint::UsbEndPoint;

        virtual void DataIn() = 0;
    };

    class UsbBulkEndPointOut
        : public UsbEndPointOut
    {
    public:
        UsbBulkEndPointOut(UsbDevice& device, uint8_t endPointNumber, uint16_t maxPacketSize);
    };

    class UsbBulkEndPointIn
        : public UsbEndPointIn
    {
    public:
        UsbBulkEndPointIn(UsbDevice& device, uint8_t endPointNumber, uint16_t maxPacketSize);
    };
}

#endif
