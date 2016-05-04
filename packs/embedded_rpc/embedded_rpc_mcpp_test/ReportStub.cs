using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TestCommunicationMCCP
{
    class ReportStub : erpc.PacketCommunicationReport.IReportIO
    {
        private UInt16 mReportLength;
        private bool isOpen = false;
        private List<byte[]> Data = new List<byte[]>();

        public ReportStub(UInt16 reportLength)
        {
            mReportLength = reportLength;
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

        public void Send(byte[] v, UInt16 len)
        {
            byte[] c = new byte[v.Count()];
            Buffer.BlockCopy(v, 0, c, 0, v.Count());
            Data.Add(c);
        }

        public bool Receive(ref byte[] v)
        {
            if (Data.Count == 0) return false;
            Buffer.BlockCopy(Data[0], 0, v, 0, v.Count());
            Data.RemoveAt(0);
            return true;
        }
        public UInt16 GetReportLength()
        {
            return mReportLength;
        }
    }
}
