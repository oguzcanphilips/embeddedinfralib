#include "infra/stream/public/ByteOutputStream.hpp"
#include "services/usb/public/UsbDevice.hpp"
#include <algorithm>
#include <cassert>

namespace services
{
    #define USBD_MAX_NUM_CONFIGURATION 1

    #define USB_CONFIG_REMOTE_WAKEUP  2
    #define USB_CONFIG_SELF_POWERED   1

    #define USBD_IDX_LANGID_STR                            0x00
    #define USBD_IDX_MFC_STR                               0x01
    #define USBD_IDX_PRODUCT_STR                           0x02
    #define USBD_IDX_SERIAL_STR                            0x03
    #define USBD_IDX_CONFIG_STR                            0x04
    #define USBD_IDX_INTERFACE_STR                         0x05

    #define USB_FEATURE_EP_HALT       0
    #define USB_FEATURE_REMOTE_WAKEUP 1
    #define USB_FEATURE_TEST_MODE     2

    #define USB_REQ_RECIPIENT_DEVICE                       0x00
    #define USB_REQ_RECIPIENT_INTERFACE                    0x01
    #define USB_REQ_RECIPIENT_ENDPOINT                     0x02

    #define USB_MAX_EP0_SIZE       64

    namespace
    {
        constexpr uint8_t LowByte(uint16_t x)
        {
            return static_cast<uint8_t>(x);
        }

        constexpr uint8_t HighByte(uint16_t x)
        {
            return static_cast<uint8_t>(x >> 8);
        }

        constexpr uint16_t SwapByte(uint16_t x)
        {
            return (x << 8) | (x >> 8);
        }
    }
    
    const uint16_t languageId = 1033;

    alignas(4) const std::array<uint8_t, 4> languadeIdDescriptor =
    {
        4,
        usbDescriptorTypeString,
        LowByte(languageId),
        HighByte(languageId),
    };

    UsbDevice::UsbDevice(hal::UsbLinkLayer& linkLayer, const UsbConfig& config, bool selfPowered)
        : hal::UsbLinkLayerObserver(linkLayer)
        , linkLayer(linkLayer)
        , selfPowered(selfPowered)
        , config(config)
        , deviceDescriptor()
        , configurationBuffer()
    {
        deviceDescriptor = {
                0x12,                       // bLength */
                usbDescriptorTypeDevice,    // bDescriptorType*/
                0x00,                       // bcdUSB */
                0x02,
                config.deviceClass,         // bDeviceClass*/
                config.deviceSubClass,      // bDeviceSubClass*/
                config.deviceProtocol,      // bDeviceProtocol*/
                USB_MAX_EP0_SIZE,           // bMaxPacketSize*/
                LowByte(config.vid),         // idVendor*/
                HighByte(config.vid),         // idVendor*/
                LowByte(config.pid),         // idProduct*/
                HighByte(config.pid),         // idProduct*/
                LowByte(config.release),     // bcdDevice release */
                HighByte(config.release),
                USBD_IDX_MFC_STR,           // Index of manufacturer string*/
                USBD_IDX_PRODUCT_STR,       // Index of product string*/
                USBD_IDX_SERIAL_STR,        // Index of serial number string*/
                USBD_MAX_NUM_CONFIGURATION  // bNumConfigurations*/
        };

        linkLayer.OpenEndPoint(0x00, hal::UsbEndPointType::control, USB_MAX_EP0_SIZE);
        endPointOut[0].maxpacket = USB_MAX_EP0_SIZE;

        linkLayer.OpenEndPoint(0x80, hal::UsbEndPointType::control, USB_MAX_EP0_SIZE);
        endPointIn[0].maxpacket = USB_MAX_EP0_SIZE;

        /* Upon Reset call user call back */
        deviceState = DeviceState::defaultState;
    }

    UsbDevice::~UsbDevice()
    {
        deviceState = DeviceState::defaultState;
    }

    void UsbDevice::RegisterInterface(UsbInterface& interface)
    {
        interfaces.push_front(interface);
    }

    hal::UsbLinkLayer& UsbDevice::LinkLayer()
    {
        return linkLayer;
    }

    void UsbDevice::SetClassConfig(uint8_t configIndex)
    {
        interfaces.front().Configured(configIndex);
    }

    void UsbDevice::ClearClassConfig(uint8_t configIndex)
    {
        interfaces.front().Unconfigured(configIndex);
    }

    void UsbDevice::SetupStage(infra::ConstByteRange setup)
    {
        USBD_SetupReqTypedef request = ParseSetupRequest(setup);

        ep0_state = EndPointState::setup;
        ep0_data_len = request.wLength;

        switch (request.bmRequest & 0x1F)
        {
            case USB_REQ_RECIPIENT_DEVICE:
                StandardDeviceRequest(&request);
                break;

            case USB_REQ_RECIPIENT_INTERFACE:
                StandardInterfaceRequest(&request);
                break;

            case USB_REQ_RECIPIENT_ENDPOINT:
                StandardEndPointRequest(&request);
                break;

            default:
                linkLayer.StallEndPoint(request.bmRequest & 0x80);
                break;
        }
    }

    void UsbDevice::DataOutStage(uint8_t epnum, infra::ConstByteRange data)
    {
        if (epnum == 0)
        {
            if (ep0_state == EndPointState::dataOut)
            {
                EndPoint* pep = &endPointOut[0];

                if (pep->remainingLength > pep->maxpacket)
                {
                    pep->remainingLength -= pep->maxpacket;
                    ControlContinueReceive(infra::ConstByteRange(data.begin(), data.begin() + std::min(pep->remainingLength, pep->maxpacket)));
                }
                else
                {
                    if (deviceState == DeviceState::configured)
                        interfaces.front().EP0_RxReady();

                    ControlSendStatus();
                }
            }
        }
        else if (deviceState == DeviceState::configured)
            interfaces.front().DataOut(epnum);
    }

    void UsbDevice::DataInStage(uint8_t epnum, infra::ConstByteRange data)
    {
        if (epnum == 0)
        {
            if (ep0_state == EndPointState::dataIn)
            {
                EndPoint* pep = &endPointIn[0];

                if (pep->remainingLength > pep->maxpacket)
                {
                    pep->remainingLength -= pep->maxpacket;

                    ControlContinueSendData(infra::ConstByteRange(data.begin(), data.begin() + pep->remainingLength));

                    /* Prepare endpoint for premature end of transfer */
                    linkLayer.PrepareReceive(0, infra::ByteRange());
                }
                else
                {
                    /* last packet is MPS multiple, so send ZLP packet */
                    if ((pep->totalLength % pep->maxpacket == 0) && (pep->totalLength >= pep->maxpacket) && (pep->totalLength < ep0_data_len))
                    {
                        ControlContinueSendData(infra::ConstByteRange());
                        ep0_data_len = 0;

                        /* Prepare endpoint for premature end of transfer */
                        linkLayer.PrepareReceive(0, infra::ByteRange());
                    }
                    else
                    {
                        if (deviceState == DeviceState::configured)
                            interfaces.front().EP0_TxSent();

                        ControlReceiveStatus();
                    }
                }
            }
        }
        else if (deviceState == DeviceState::configured)
            interfaces.front().DataIn(epnum);
    }

    void UsbDevice::Suspend()
    {
        oldDeviceState = deviceState;
        deviceState = DeviceState::suspended;
    }

    void UsbDevice::Resume()
    {
        deviceState = oldDeviceState;
    }

    void UsbDevice::StartOfFrame()
    {
        if (deviceState == DeviceState::configured)
            interfaces.front().StartOfFrame();
    }

    void UsbDevice::IsochronousInIncomplete(uint8_t epnum)
    {}

    void UsbDevice::IsochronousOutIncomplete(uint8_t epnum)
    {}

    void UsbDevice::ControlSendData(infra::ConstByteRange data)
    {
        ep0_state = EndPointState::dataIn;
        endPointIn[0].totalLength = data.size();
        endPointIn[0].remainingLength = data.size();

        linkLayer.Transmit(0x00, data);
    }

    void UsbDevice::ControlPrepareReceive(infra::ByteRange data)
    {
        ep0_state = EndPointState::dataOut;
        endPointOut[0].totalLength = data.size();
        endPointOut[0].remainingLength = data.size();
        /* Start the transfer */
        linkLayer.PrepareReceive(0, data);
    }

    void UsbDevice::ControlError()
    {
        linkLayer.StallEndPoint(0x80);
        linkLayer.StallEndPoint(0);
    }

    bool UsbDevice::Configured() const
    {
        return deviceState == DeviceState::configured;
    }

    void UsbDevice::StandardDeviceRequest(USBD_SetupReqTypedef *req)
    {
        switch (req->bRequest)
        {
            case usbRequestGetDescriptor:
                GetDescriptor(req->wValue, req->wLength);
                break;

            case usbRequestSetAddress:
                SetAddress(req->wValue, req->wIndex, req->wLength);
                break;

            case usbRequestSetConfiguration:
                SetConfig(req->wValue);
                break;

            case usbRequestGetConfiguration:
                GetConfig(req->wLength);
                break;

            case usbRequestGetStatus:
                GetStatus();
                break;

            case usbRequestSetFeature:
                SetFeature(req->wValue);
                break;

            case usbRequestClearFeature:
                ClearFeature(req->wValue);
                break;

            default:
                ControlError();
                break;
        }
    }

    void UsbDevice::StandardInterfaceRequest(USBD_SetupReqTypedef  *req)
    {
        if (LowByte(req->wIndex) < std::distance(interfaces.begin(), interfaces.end()) && deviceState == DeviceState::configured)
        {
            std::next(interfaces.begin(), LowByte(req->wIndex))->Setup(req);

            if (req->wLength == 0)
                ControlSendStatus();
        }
        else
            ControlError();
    }

    void UsbDevice::StandardEndPointRequest(USBD_SetupReqTypedef  *req)
    {
        uint8_t ep_addr;
        EndPoint* pep;
        ep_addr = LowByte(req->wIndex);

        /* Check if it is a class request */
        if ((req->bmRequest & 0x60) == 0x20)
        {
            interfaces.front().Setup(req);

            return;
        }

        switch (req->bRequest)
        {
            case usbRequestSetFeature:
                switch (deviceState)
                {
                    case DeviceState::addressed:
                        if (ep_addr != 0x00 && ep_addr != 0x80)
                            linkLayer.StallEndPoint(ep_addr);
                        break;

                    case DeviceState::configured:
                        if (req->wValue == USB_FEATURE_EP_HALT)
                        {
                            if (ep_addr != 0x00 && ep_addr != 0x80)
                                linkLayer.StallEndPoint(ep_addr);
                        }
                        interfaces.front().Setup(req);
                        ControlSendStatus();
                        break;

                    default:
                        ControlError();
                        break;
                }
                break;

            case usbRequestClearFeature:
                switch (deviceState)
                {
                    case DeviceState::addressed:
                        if (ep_addr != 0x00 && ep_addr != 0x80)
                            linkLayer.StallEndPoint(ep_addr);
                        break;

                    case DeviceState::configured:
                        if (req->wValue == USB_FEATURE_EP_HALT)
                        {
                            if ((ep_addr & 0x7F) != 0x00)
                            {
                                linkLayer.ClearStallEndPoint(ep_addr);
                                interfaces.front().Setup(req);
                            }
                            ControlSendStatus();
                        }
                        break;

                    default:
                        ControlError();
                        break;
                }
                break;

            case usbRequestGetStatus:
                switch (deviceState)
                {
                    case DeviceState::addressed:
                        if ((ep_addr & 0x7F) != 0x00)
                            linkLayer.StallEndPoint(ep_addr);
                        break;

                    case DeviceState::configured:
                        pep = ((ep_addr & 0x80) == 0x80) ? &endPointIn[ep_addr & 0x7F] : &endPointOut[ep_addr & 0x7F];
                        if (linkLayer.IsStallEndPoint(ep_addr))
                            pep->status = 0x0001;
                        else
                            pep->status = 0x0000;

                        ControlSendData(infra::MakeByteRange(pep->status));
                        break;

                    default:
                        ControlError();
                        break;
                }
                break;

            default:
                break;
        }
    }

    void UsbDevice::GetDescriptor(uint16_t value, uint16_t length)
    {
        infra::ConstByteRange buffer;

        switch (value >> 8)
        {
            case usbDescriptorTypeDevice:
                buffer = infra::MakeByteRange(deviceDescriptor);
                break;

            case usbDescriptorTypeConfiguration:
                buffer = GetConfigurationDescriptor();
                assert(buffer[1] == usbDescriptorTypeConfiguration);
                break;

            case usbDescriptorTypeString:
                switch (static_cast<uint8_t>(value))
                {
                    case USBD_IDX_LANGID_STR:
                        buffer = infra::MakeByteRange(languadeIdDescriptor);
                        break;

                    case USBD_IDX_MFC_STR:
                        buffer = config.manufacturerString;
                        break;

                    case USBD_IDX_PRODUCT_STR:
                        buffer = config.productString;
                        break;

                    case USBD_IDX_SERIAL_STR:
                        buffer = config.serialNumber;
                        break;

                    case USBD_IDX_CONFIG_STR:
                        buffer = config.configuration;
                        break;

                    case USBD_IDX_INTERFACE_STR:
                        buffer = config.interfaceString;
                        break;

                    default:
                        ControlError();
                        return;
                }
                break;

            case usbDescriptorTypeDeviceQualifier:
                if (deviceSpeed == hal::UsbSpeed::high)
                {
                    #define  deviceQualifierDescriptorLength 0x0A
                    alignas(4) static uint8_t deviceQualifierDesc[deviceQualifierDescriptorLength] =
                    {
                        deviceQualifierDescriptorLength,
                        usbDescriptorTypeDeviceQualifier,
                        0x00, 0x02,         // USB version
                        0x00, 0x00, 0x00,   // Device class, subclass, protocol
                        0x40,               // Max packet size end point 0
                        0x01,               // Number of configurations
                        0x00                // Reserved
                    };

                    buffer = infra::MakeByteRange(deviceQualifierDesc);
                    break;
                }
                else
                {
                    ControlError();
                    return;
                }

            case usbDescriptorTypeOtherSpeedConfiguration:
                if (deviceSpeed == hal::UsbSpeed::high)
                {
                    abort();    // Not implemented
                    //buffer = interfaces.front().GetOtherSpeedConfigDescriptor();
                    assert(buffer[1] == usbDescriptorTypeOtherSpeedConfiguration);
                    break;
                }
                else
                {
                    ControlError();
                    return;
                }

            default:
                ControlError();
                return;
        }

        if (!buffer.empty() && length != 0)
        {
            buffer.shrink_from_back_to(length);
            ControlSendData(buffer);
        }
    }

    void UsbDevice::SetAddress(uint16_t value, uint16_t index, uint16_t length)
    {
        if (index == 0 && length == 0)
        {
            if (deviceState == DeviceState::configured)
                ControlError();
            else
            {
                uint8_t dev_addr = static_cast<uint8_t>(value) & 0x7F;
                linkLayer.SetUsbAddress(dev_addr);
                ControlSendStatus();

                if (dev_addr != 0)
                    deviceState = DeviceState::addressed;
                else
                    deviceState = DeviceState::defaultState;
            }
        }
        else
            ControlError();
    }

    void UsbDevice::SetConfig(uint16_t value)
    {
        uint8_t cfgidx = static_cast<uint8_t>(value);

        if (cfgidx > USBD_MAX_NUM_CONFIGURATION)
            ControlError();
        else
        {
            switch (deviceState)
            {
                case DeviceState::addressed:
                    if (cfgidx != 0)
                    {
                        dev_config = cfgidx;
                        deviceState = DeviceState::configured;
                        SetClassConfig(cfgidx);
                    }

                    ControlSendStatus();
                    break;

                case DeviceState::configured:
                    if (cfgidx == 0)
                    {
                        deviceState = DeviceState::addressed;
                        dev_config = cfgidx;
                        ClearClassConfig(cfgidx);
                    }
                    else if (cfgidx != dev_config)
                    {
                        /* Clear old configuration */
                        ClearClassConfig(dev_config);

                        /* set new configuration */
                        dev_config = cfgidx;
                        SetClassConfig(cfgidx);
                    }

                    ControlSendStatus();
                    break;

                default:
                    ControlError();
                    break;
            }
        }
    }

    void UsbDevice::GetConfig(uint16_t length)
    {
        if (length != 1)
        {
            ControlError();
        }
        else
        {
            switch (deviceState)
            {
                case DeviceState::addressed:
                    dev_default_config = 0;
                    ControlSendData(infra::MakeByteRange(dev_default_config));
                    break;

                case DeviceState::configured:
                    ControlSendData(infra::MakeByteRange(dev_config));
                    break;

                default:
                    ControlError();
                    break;
            }
        }
    }

    void UsbDevice::GetStatus()
    {
        switch (deviceState)
        {
            case DeviceState::addressed:
            case DeviceState::configured:
                if (selfPowered)
                    dev_config_status = USB_CONFIG_SELF_POWERED;
                else
                    dev_config_status = 0;

                if (remoteWakeUp)
                    dev_config_status |= USB_CONFIG_REMOTE_WAKEUP;

                ControlSendData(infra::MakeByteRange(dev_config_status));
                break;

            default:
                ControlError();
                break;
        }
    }

    void UsbDevice::SetFeature(uint16_t value)
    {
        if (value == USB_FEATURE_REMOTE_WAKEUP)
        {
            remoteWakeUp = true;
            ControlSendStatus();
        }
    }

    void UsbDevice::ClearFeature(uint16_t value)
    {
        switch (deviceState)
        {
            case DeviceState::addressed:
            case DeviceState::configured:
                if (value == USB_FEATURE_REMOTE_WAKEUP)
                {
                    remoteWakeUp = false;
                    ControlSendStatus();
                }
                break;

            default:
                ControlError();
                break;
        }
    }

    USBD_SetupReqTypedef UsbDevice::ParseSetupRequest(infra::ConstByteRange setup) const
    {
        USBD_SetupReqTypedef result;

        result.bmRequest = setup[0];
        result.bRequest = setup[1];
        result.wValue = SwapByte(setup[2]);
        result.wIndex = SwapByte(setup[4]);
        result.wLength = SwapByte(setup[6]);

        return result;
    }

    void  UsbDevice::ControlContinueSendData(infra::ConstByteRange data)
    {
        linkLayer.Transmit(0x00, data);
    }

    void  UsbDevice::ControlContinueReceive(infra::ConstByteRange data)
    {
        linkLayer.PrepareReceive(0, data);
    }

    void  UsbDevice::ControlSendStatus()
    {
        ep0_state = EndPointState::statusIn;
        linkLayer.Transmit(0x00, infra::ConstByteRange());
    }

    void UsbDevice::ControlReceiveStatus()
    {
        ep0_state = EndPointState::statusOut;
        linkLayer.PrepareReceive(0, infra::ByteRange());
    }

    infra::ByteRange UsbDevice::GetConfigurationDescriptor()
    {
        static const std::array<uint8_t, 9> configurationDescriptor =
        {
            0x09,           // bLength: Length of this descriptor
            usbDescriptorTypeConfiguration,     // bDescriptorType
            0x00, 0x00,     // wTotalLength, little endian
            0x00,           // bNumInterfaces
            0x01,           // bConfigurationValue: Value to use as an argument to select this configuration
            0x00,           // iConfiguration: Index of string descriptor describing the configuration
            0xC0,           // bmAttributes: Self powered, no remote wakeup
            0x32,           // bMaxPower: 2 * 50 mA
        };

        infra::ByteOutputStream stream(configurationBuffer);
        
        stream << configurationDescriptor;

        for (const UsbInterface& interface : interfaces)
            interface.AddConfigDescriptor(stream);

        configurationBuffer[2] = static_cast<uint8_t>(stream.Processed().size());
        configurationBuffer[3] = static_cast<uint8_t>(stream.Processed().size() >> 8);
        
        for (const UsbInterface& interface : interfaces)
            configurationBuffer[4] += interface.NumberOfInterfaces();

        return stream.Processed();
    }
}
