#include "services/usb/UsbEndPoint.hpp"

namespace services
{
    UsbEndPoint::UsbEndPoint(UsbDevice& device, uint8_t endPointNumber, hal::UsbEndPointType type, uint16_t maxPacketSize)
        : device(device)
        , endPointNumber(endPointNumber)
    {
        device.LinkLayer().OpenEndPoint(endPointNumber, type, maxPacketSize);
    }

    UsbEndPoint::~UsbEndPoint()
    {
        device.LinkLayer().CloseEndPoint(endPointNumber);
    }

    UsbDevice& UsbEndPoint::Device()
    {
        return device;
    }

    uint8_t UsbEndPoint::EndPointNumber() const
    {
        return endPointNumber;
    }

    UsbBulkEndPointOut::UsbBulkEndPointOut(UsbDevice& device, uint8_t endPointNumber, uint16_t maxPacketSize)
        : UsbEndPointOut(device, endPointNumber, hal::UsbEndPointType::bulk, maxPacketSize)
    {}

    UsbBulkEndPointIn::UsbBulkEndPointIn(UsbDevice& device, uint8_t endPointNumber, uint16_t maxPacketSize)
        : UsbEndPointIn(device, endPointNumber, hal::UsbEndPointType::bulk, maxPacketSize)
    {}
}
