using System;
using System.Diagnostics;
using PInS.HID;

namespace PInS.RPC
{
    public class PacketLinkHID : PacketCommunicationReport.IReportIO, IDisposable
    {
        private UInt16 mReportLength;
        private bool isOpen = false;

        // we need a separate 'handle' for reading and writing in parallel
        public HidDevice hidDeviceWr { get; private set; }
        public HidDevice hidDeviceRd { get; private set; }

        public PacketLinkHID(HidDevice hidDev)
        {
            SharedConstruction(hidDev);
        }

        public PacketLinkHID(uint vendorId, ushort productId)
        {
            var e = new HidDeviceEnumerator(vendorId, productId);
            var ok = e.MoveNext();
            Debug.Assert(ok);

            SharedConstruction(e.Current);
        }

        private void SharedConstruction(HidDevice hidDev)
        {
            hidDeviceWr = hidDev;
            hidDeviceRd = new HidDevice(hidDeviceWr.DevicePath);

            Open();
            Debug.Assert(hidDeviceWr.InReportLength == hidDeviceWr.OutReportLength);
            mReportLength = (ushort) (hidDeviceWr.InReportLength);
        }

        public void Open()
        {
            if (!isOpen && hidDeviceWr.Connect())
            {
                if (hidDeviceRd.Connect()) isOpen = true;
                else                       hidDeviceWr.Disconnect();
            }
        }

        public bool IsOpen()
        {
            return isOpen;
        }

        public void Close()
        {
            if (isOpen)
            {
                isOpen = false;
                hidDeviceRd.Disconnect();
                hidDeviceWr.Disconnect();
            }
        }

        public void Send(byte[] v, UInt16 len)
        {
            hidDeviceWr.SendOutputReport(v);
        }

        public bool Receive(ref byte[] v)
        {
            return hidDeviceRd.GetInputReport(out v);
        }

        public UInt16 GetReportLength()
        {
            return mReportLength;
        }

        public void Dispose()
        {
            if (isOpen) Close();
        }
    }
}
