using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TestCommunicationMCCP
{
    class BarImpl : IBar
    {
        private KeyId ki = new KeyId();
        private UInt16 value;
        public KeyId GetKeyId()
        {
            Console.WriteLine("Bar.GetKeyId");
            return ki;
        }

        public void SetKeyId(KeyId kid)
        {
            Console.WriteLine("Bar.SetKeyId");
            ki = kid;
        }

        public bool IO_In(ushort v)
        {
            Console.WriteLine("Bar.IO_In");
            value = v;
            return true;
        }

        public bool IO_Out(ref ushort v)
        {
            Console.WriteLine("Bar.IO_Out");
            v = value;
            return true;
        }

        public void IO_InOut(ref ushort v)
        {
            Console.WriteLine("Bar.IO_InOut");
            UInt16 tmp = value;
            value = v;
            v = tmp;
        }
    }
}
