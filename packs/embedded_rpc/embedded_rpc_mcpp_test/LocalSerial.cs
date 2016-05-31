using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace TestCommunicationMCCP
{
    class LocalSerial : erpc.PacketCommunicationSLIP.ISerialIO
    {
        private static int idnew = 0;
        private int id = idnew++;
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
            Console.WriteLine(id + " Send:" + v);
            lock (link.data)
            {
                link.data.Add(v);
            }
        }

        public int GetCount()
        {
            lock (data)
            {
                return data.Count;
            }
        }
        public bool Receive(ref byte v)
        {
            Console.WriteLine(id + " Receive....");
            int timeout = 250;
            while (GetCount() == 0)
            {
                Thread.Sleep(10);
                if (0 == timeout--) break;
            }
            lock (data)
            {
                if (data.Count == 0) return false;
                v = data[0];
                data.RemoveAt(0);
                Console.WriteLine(id+" Receive:" + v);
                return true;
            }
        }
    }
}