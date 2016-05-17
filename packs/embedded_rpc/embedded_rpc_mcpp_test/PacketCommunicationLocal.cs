using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace TestCommunicationMCCP
{
    class PacketCommunicationLocal : erpc.PacketCommunication
    {
        public List<UInt16> mPacket = new List<UInt16>();
        public List<byte> mPacketRet = new List<byte>();
        public override void PackedEndToken()
        {
            mPacket.Add(0x101);
            Start();
        }

        public override void PacketStartToken()
        {
            mPacket.Add(0x100);
        }

        public override void ProcessReceive()
        {
            mPacketRet.Add((byte)(mPacket[1] - 128));
            mPacketRet.Add((byte)mPacket[2]);
            mPacketRet.Add((byte)mPacket[3]);

            mPacketRet.Add((byte)mPacket[4]);
            mPacketRet.Add((byte)mPacket[5]);
            mPacketRet.Add((byte)mPacket[6]);
            mPacketRet.Add((byte)mPacket[7]);
            if (mPacket.Count == 11) // CRC
            {
                mPacketRet.Add(0x5f);
                mPacketRet.Add(0x8f);
            }
            else if (mPacket.Count == 10) // CHECKSUM
            {
                mPacketRet.Add(24);
            }
            Receive();

        }

        public override bool IsPacketEnded()
        {
            return true;
        }

        public override bool ReadByte(ref byte v)
        {
            if (mPacketRet.Count == 0)
            {
                return false;
            }
            v = mPacketRet[0];
            mPacketRet.RemoveAt(0);
            return true;
        }
        public override void WriteByte(byte v)
        {
            mPacket.Add(v);
        }
    }
}
