#ifndef SERVICES_USB_INTERFACE_HPP
#define SERVICES_USB_INTERFACE_HPP

#include "infra/stream/OutputStream.hpp"
#include "infra/util/ByteRange.hpp"
#include "infra/util/IntrusiveForwardList.hpp"
#include <cstdint>

namespace services
{
    struct USBD_SetupReqTypedef
    {
        uint8_t bmRequest;
        uint8_t bRequest;
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
    };

    static const uint8_t usbRequestTypeStandard = 0;
    static const uint8_t usbRequestTypeClass = 0x20;
    static const uint8_t usbRequestTypeVendor = 0x40;
    static const uint8_t usbRequestTypeMask = 0x60;

    class UsbInterface
        : public infra::IntrusiveForwardList<UsbInterface>::NodeType
    {
    public:
        virtual void Configured(uint8_t configIndex) = 0;
        virtual void Unconfigured(uint8_t configIndex) = 0;

        // Control Endpoints
        virtual void Setup(USBD_SetupReqTypedef* req) = 0;
        virtual void EP0_TxSent() = 0;
        virtual void EP0_RxReady() = 0;

        // Class Specific Endpoints
        virtual void DataIn(uint8_t epnum) = 0;
        virtual void DataOut(uint8_t epnum) = 0;
        virtual void StartOfFrame() = 0;
        virtual void IsoINIncomplete(uint8_t epnum) = 0;
        virtual void IsoOUTIncomplete(uint8_t epnum) = 0;

        virtual uint8_t NumberOfInterfaces() const = 0;
        virtual void AddConfigDescriptor(infra::DataOutputStream& stream) const = 0;
    };
}

#endif
