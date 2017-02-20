#include "services/usb/public/UsbInterfaceVirtualComPort.hpp"
#include "infra/event/public/EventDispatcher.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>

namespace services
{
#define LOBYTE(x)  ((uint8_t)(x & 0x00FF))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00) >>8))
#define CDC_CMD_PACKET_SIZE 8

    namespace
    {
        alignas(4) uint8_t virtualComPortConfigurationDescriptor[58] =
        {
            /*Interface Descriptor */
            0x09,   /* bLength: Interface Descriptor size */
            usbDescriptorTypeInterface,  /* bDescriptorType: Interface */
            /* Interface descriptor type */
            0x00,   /* bInterfaceNumber: Number of Interface */
            0x00,   /* bAlternateSetting: Alternate setting */
            0x01,   /* bNumEndpoints: One endpoints used */
            0x02,   /* bInterfaceClass: Communication Interface Class */
            0x02,   /* bInterfaceSubClass: Abstract Control Model */
            0x01,   /* bInterfaceProtocol: Common AT commands */
            0x00,   /* iInterface: */

            /*Header Functional Descriptor*/
            0x05,   /* bLength: Endpoint Descriptor size */
            0x24,   /* bDescriptorType: CS_INTERFACE */
            0x00,   /* bDescriptorSubtype: Header Func Desc */
            0x10,   /* bcdCDC: spec release number */
            0x01,

            /*Call Management Functional Descriptor*/
            0x05,   /* bFunctionLength */
            0x24,   /* bDescriptorType: CS_INTERFACE */
            0x01,   /* bDescriptorSubtype: Call Management Func Desc */
            0x00,   /* bmCapabilities: D0+D1 */
            0x01,   /* bDataInterface: 1 */

            /*ACM Functional Descriptor*/
            0x04,   /* bFunctionLength */
            0x24,   /* bDescriptorType: CS_INTERFACE */
            0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
            0x02,   /* bmCapabilities */

            /*Union Functional Descriptor*/
            0x05,   /* bFunctionLength */
            0x24,   /* bDescriptorType: CS_INTERFACE */
            0x06,   /* bDescriptorSubtype: Union func desc */
            0x00,   /* bMasterInterface: Communication class interface */
            0x01,   /* bSlaveInterface0: Data Class Interface */

            /*Endpoint 2 Descriptor*/
            0x07,                           /* bLength: Endpoint Descriptor size */
            usbDescriptorTypeEndPoint,   /* bDescriptorType: Endpoint */
            CDC_CMD_EP,                     /* bEndpointAddress */
            0x03,                           /* bmAttributes: Interrupt */
            LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
            HIBYTE(CDC_CMD_PACKET_SIZE),
            0xff,                           /* bInterval: */
            /*---------------------------------------------------------------------------*/

            /*Data class interface descriptor*/
            0x09,   /* bLength: Endpoint Descriptor size */
            usbDescriptorTypeInterface,  /* bDescriptorType: */
            0x01,   /* bInterfaceNumber: Number of Interface */
            0x00,   /* bAlternateSetting: Alternate setting */
            0x02,   /* bNumEndpoints: Two endpoints used */
            0x0A,   /* bInterfaceClass: CDC */
            0x00,   /* bInterfaceSubClass: */
            0x00,   /* bInterfaceProtocol: */
            0x00,   /* iInterface: */

            /*Endpoint OUT Descriptor*/
            0x07,   /* bLength: Endpoint Descriptor size */
            usbDescriptorTypeEndPoint,      /* bDescriptorType: Endpoint */
            CDC_OUT_EP,                        /* bEndpointAddress */
            0x02,                              /* bmAttributes: Bulk */
            LOBYTE(fullSpeedMaxPacketSize),  /* wMaxPacketSize: */
            HIBYTE(fullSpeedMaxPacketSize),
            0x00,                              /* bInterval: ignore for Bulk transfer */

            /*Endpoint IN Descriptor*/
            0x07,   /* bLength: Endpoint Descriptor size */
            usbDescriptorTypeEndPoint,      /* bDescriptorType: Endpoint */
            CDC_IN_EP,                         /* bEndpointAddress */
            0x02,                              /* bmAttributes: Bulk */
            LOBYTE(fullSpeedMaxPacketSize),  /* wMaxPacketSize: */
            HIBYTE(fullSpeedMaxPacketSize),
            0x00                               /* bInterval: ignore for Bulk transfer */
        };
    }

    UsbInterfaceVirtualComPort::UsbInterfaceVirtualComPort(UsbDevice& device)
        : device(device)
        , data()
        , lineCoding({
            115200, // baud rate
            0x00,   // stop bits = 1
            0x00,   // parity = none
            0x08    // nb. of bits = 8
        })
    {
        device.RegisterInterface(*this);
    }

    void UsbInterfaceVirtualComPort::SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion)
    {
        assert(configured);
        configured->dataEndPointIn.SendData(data, actionOnCompletion);
    }

    void UsbInterfaceVirtualComPort::ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived)
    {
        assert(!configured);
        this->dataReceived = dataReceived;
    }

    void UsbInterfaceVirtualComPort::Configured(uint8_t configIndex)
    {
        configured.Emplace(device, dataReceived);

        device.LinkLayer().OpenEndPoint(CDC_CMD_EP, hal::UsbEndPointType::interrupt, CDC_CMD_PACKET_SIZE);
    }

    void UsbInterfaceVirtualComPort::Unconfigured(uint8_t configIndex)
    {
        configured = infra::none;

        device.LinkLayer().CloseEndPoint(CDC_CMD_EP);
    }

    void UsbInterfaceVirtualComPort::Setup(USBD_SetupReqTypedef* req)
    {
        static uint8_t alternateInterface = 0;

        switch (req->bmRequest & usbRequestTypeMask)
        {
            case usbRequestTypeClass:
                if (req->wLength != 0)
                {
                    if ((req->bmRequest & 0x80) != 0)
                    {
                        Control(req->bRequest, data.data(), req->wLength);
                        device.ControlSendData(infra::ByteRange(data.data(), data.data() + req->wLength));
                    }
                    else
                    {
                        cmdOpCode = req->bRequest;
                        cmdLength = static_cast<uint8_t>(req->wLength);

                        device.ControlPrepareReceive(infra::ByteRange(data.data(), data.data() + req->wLength));
                    }
                }
                else
                {
                    Control(req->bRequest, (uint8_t*)req, 0);
                }
                break;

            case usbRequestTypeStandard:
                switch (req->bRequest)
                {
                    case usbRequestGetInterface:
                        device.ControlSendData(infra::MakeByteRange(alternateInterface));
                        break;
                    case usbRequestSetInterface:
                        break;
                }
                break;
        }
    }

    void UsbInterfaceVirtualComPort::EP0_RxReady()
    {
        if (cmdOpCode != 0xFF)
        {
            Control(cmdOpCode, data.data(), cmdLength);
            cmdOpCode = 0xFF;
        }
    }

    void UsbInterfaceVirtualComPort::DataIn(uint8_t epnum)
    {
        if ((epnum | 0x80) == CDC_IN_EP)
            configured->dataEndPointIn.DataIn();
    }

    void UsbInterfaceVirtualComPort::DataOut(uint8_t epnum)
    {
        if (epnum == CDC_OUT_EP)
            configured->dataEndPointOut.DataOut();
    }

    uint8_t UsbInterfaceVirtualComPort::NumberOfInterfaces() const
    {
        return 2;
    }

    void UsbInterfaceVirtualComPort::AddConfigDescriptor(infra::DataOutputStream& stream) const
    {
        stream << virtualComPortConfigurationDescriptor;
    }

    void UsbInterfaceVirtualComPort::Control(uint8_t cmd, uint8_t* pbuf, uint16_t length)
    {
        switch (cmd)
        {
            case cdcSendEncapsulatedCommand:
                break;
            case cdcGetEncapsulatedResponse:
                break;
            case cdcSetCommFeature:
                break;
            case cdcGetCommFeature:
                break;
            case cdcClearCommFeature:
                break;
            case cdcSetLineCoding:
                lineCoding.bitrate = static_cast<uint32_t>(pbuf[0] | (pbuf[1] << 8) | (pbuf[2] << 16) | (pbuf[3] << 24));
                lineCoding.format = pbuf[4];
                lineCoding.paritytype = pbuf[5];
                lineCoding.datatype = pbuf[6];
                break;
            case cdcGetLineCoding:
                pbuf[0] = static_cast<uint8_t>(lineCoding.bitrate);
                pbuf[1] = static_cast<uint8_t>(lineCoding.bitrate >> 8);
                pbuf[2] = static_cast<uint8_t>(lineCoding.bitrate >> 16);
                pbuf[3] = static_cast<uint8_t>(lineCoding.bitrate >> 24);
                pbuf[4] = lineCoding.format;
                pbuf[5] = lineCoding.paritytype;
                pbuf[6] = lineCoding.datatype;
                break;
            case cdcSetControlLineState:
                break;
            case cdcSendBreak:
                break;
            default:
                break;
        }
    }

    UsbInterfaceVirtualComPort::ConfiguredVirtualComPort::ConfiguredVirtualComPort(UsbDevice& device, infra::Function<void(infra::ConstByteRange data)> dataReceived)
        : dataEndPointOut(device, CDC_OUT_EP, dataReceived)
        , dataEndPointIn(device, CDC_IN_EP)
    {}

    UsbInterfaceVirtualComPort::ConfiguredVirtualComPort::DataEndPointOut::DataEndPointOut(UsbDevice& device, uint8_t endPointNumber, const infra::Function<void(infra::ConstByteRange data)>& dataReceived)
        : UsbBulkEndPointOut(device, endPointNumber, static_cast<uint16_t>(rxBuffer.size()))
        , dataReceived(dataReceived)
    {
        PrepareReceive();
    }

    void UsbInterfaceVirtualComPort::ConfiguredVirtualComPort::DataEndPointOut::DataOut()
    {
        dataReceived(infra::ByteRange(rxBuffer.data(), rxBuffer.data() + Device().LinkLayer().GetReceiveDataSize(EndPointNumber())));
        PrepareReceive();
    }

    void UsbInterfaceVirtualComPort::ConfiguredVirtualComPort::DataEndPointOut::PrepareReceive()
    {
        Device().LinkLayer().PrepareReceive(EndPointNumber(), rxBuffer);
    }

    UsbInterfaceVirtualComPort::ConfiguredVirtualComPort::DataEndPointIn::DataEndPointIn(UsbDevice& device, uint8_t endPointNumber)
        : UsbBulkEndPointIn(device, endPointNumber, fullSpeedMaxPacketSize)
    {}

    void UsbInterfaceVirtualComPort::ConfiguredVirtualComPort::DataEndPointIn::SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion)
    {
        assert(!sending);
        sending = true;

        onTransmitComplete = actionOnCompletion;
        Device().LinkLayer().Transmit(EndPointNumber(), data);
    }

    void UsbInterfaceVirtualComPort::ConfiguredVirtualComPort::DataEndPointIn::DataIn()
    {
        sending = false;
        onTransmitComplete();
    }
}
