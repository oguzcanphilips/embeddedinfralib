using System;
using NUnit.Framework;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TestCommunicationMCCP
{
    [TestFixture]
    public class TestProxy
    {
        [TestFixtureSetUp]
        public void SetUpFixture()
        {
        }

        [TestFixtureTearDown]
        public void TearDownFixture()
        {
        }

        [Test]
        public void CreateProxy()
        {
            PacketCommunicationLocal pc = new PacketCommunicationLocal();

            erpc.IFoo foo = new erpc.FooProxy(pc);
            UInt32 res = foo.Post(0x12345678);

            Assert.AreEqual(0x100, pc.mPacket[0]);
            Assert.AreEqual(    2, pc.mPacket[1]);
            Assert.AreEqual(    2, pc.mPacket[2]);

            Assert.AreEqual( 0x12, pc.mPacket[4]);
            Assert.AreEqual( 0x34, pc.mPacket[5]);
            Assert.AreEqual( 0x56, pc.mPacket[6]);
            Assert.AreEqual( 0x78, pc.mPacket[7]);

            if (pc.mPacket.Count == 9) // no validation
            {
            }
            else if (pc.mPacket.Count == 10) 
            {
                Assert.AreEqual(152, pc.mPacket[8]); // CHECKSUM                
            }
            else if (pc.mPacket.Count == 11) 
            {
                Assert.AreEqual(95, pc.mPacket[8]); // CRC
                Assert.AreEqual(143, pc.mPacket[9]);
            }
            else
            {
                Assert.AreEqual(11, pc.mPacket.Count);
            }
            Assert.AreEqual(0x101, pc.mPacket[pc.mPacket.Count-1]);
            Assert.AreEqual(0x12345678, res);
        }

        [Test]
        [ExpectedException("System.Exception")]
        public void CreateProxyTwice()
        {
            PacketCommunicationLocal pc = new PacketCommunicationLocal();

            erpc.IFoo foo = new erpc.FooProxy(pc);
            erpc.IFoo foo2 = new erpc.FooProxy(pc);
        }

        [Test]
        public void CreateProxyDestroyCreate()
        {
            PacketCommunicationLocal pc = new PacketCommunicationLocal();

            erpc.FooProxy foo = new erpc.FooProxy(pc);
            foo.Dispose();
            erpc.IFoo foo2 = new erpc.FooProxy(pc);
        }

        [Test]
        public void SerialData()
        {
            SerialStub serial = new SerialStub();
            erpc.PacketCommunicationSLIP pc = new erpc.PacketCommunicationSLIP(serial);
            pc.Stop();
            pc.WriteStartToken();
            for (int i = 0; i <= 255; i++)
            {
                pc.WriteByte((byte) i);
            }
            pc.WriteEndToken();

            pc.ProcessReceive(); // find begin and reads one byte
            for (int i = 1; i <= 255; i++)
            {
                byte v = 0;
                pc.ReadByte(ref v);
                Assert.AreEqual((byte)i, v);
            }
            Assert.True(pc.IsPacketEnded());
        }
        [Test]
        public void ReportData()
        {
            ReportStub report = new ReportStub(4);
            erpc.PacketCommunicationReport pc = new erpc.PacketCommunicationReport(report);
            pc.Stop();
            pc.WriteStartToken();
            for (int i = 0; i <= 255; i++)
            {
                pc.WriteByte((byte)i);
            }
            pc.WriteEndToken();

            pc.ProcessReceive(); // find begin and reads one byte
            for (int i = 1; i <= 255; i++)
            {
                byte v = 0;
                pc.ReadByte(ref v);
                Assert.AreEqual((byte)i, v);
            }
            Assert.True(pc.IsPacketEnded());
        }

        [Test]
        public void CallImpl()
        {
            LocalSerial serialA = new LocalSerial();
            LocalSerial serialB = serialA.CreateLink();
            serialA.Open();
            serialB.Open();
            erpc.PacketCommunicationSLIP pcA = new erpc.PacketCommunicationSLIP(serialA);
            erpc.PacketCommunicationSLIP pcB = new erpc.PacketCommunicationSLIP(serialB);

            erpc.BarProxy proxy = new erpc.BarProxy(pcA);
            erpc.BarSkeleton skeleton = new erpc.BarSkeleton(pcB);
            skeleton.SetImpl(new BarImpl());

            proxy.IO_In(100);
            UInt16 vOut = 0;
            proxy.IO_Out(ref vOut);
            Assert.AreEqual(100, vOut);

            vOut = 200;
            proxy.IO_InOut(ref vOut);
            Assert.AreEqual(100, vOut);
            proxy.IO_Out(ref vOut);
            Assert.AreEqual(200, vOut);
        }
    }
}

