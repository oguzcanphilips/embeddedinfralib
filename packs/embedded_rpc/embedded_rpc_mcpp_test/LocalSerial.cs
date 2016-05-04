using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace TestCommunicationMCCP
{
    class LocalSerial : PacketCommunicationSLIP.ISerialIO
    {
        private bool isOpen = false;
        private List<byte> data = new List<byte>();
        private LocalSerial link;

        public LocalSerial CreateLink()
        {
            LocalSerial ls = new LocalSerial();
            link = ls;
            ls.link = this;
            return ls;
        }

        public void Open()
        {
            isOpen = true;
        }
        public bool IsOpen()
        {
            return isOpen;
        }

        public void Close()
        {
            isOpen = false;
        }

        public void Send(byte v)
        {
            lock (link.data)
            {
                link.data.Add(v);
            }
        }

        public bool Receive(ref byte v)
        {
            int timeout = 250;
            while (data.Count == 0)
            {
                Thread.Sleep(10);
                if (0 == timeout--) break;
            }
            if (data.Count == 0) return false;
            lock (data)
            {
                v = data[0];
                data.RemoveAt(0);
                return true;
            }
        }
    }
}