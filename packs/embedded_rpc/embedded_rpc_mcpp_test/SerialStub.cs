using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TestCommunicationMCCP
{
    class SerialStub : PacketCommunicationSLIP.ISerialIO
    {
        private bool isOpen = false;
        private List<byte> Data = new List<byte>();
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
            Data.Add(v);
        }

        public bool Receive(ref byte v)
        {
            if (Data.Count == 0) return false;
            v = Data[0];
            Data.RemoveAt(0);
            return true;
        }
    }
}
