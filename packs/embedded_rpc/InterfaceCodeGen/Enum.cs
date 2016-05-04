using System;
using System.Collections.Generic;

namespace InterfaceCodeGen
{
    class Enum
    {
        public class Field
        {
            private readonly string mName;
            private readonly Int32  mValue;

            public string Name  { get { return mName; } }
            public Int32  Value { get { return mValue; } }

            public Field(string name, Int32 value)
            {
                mName = name;
                mValue = value;
            }
        }

        private readonly string mName;
        private readonly List<Field> mFields = new List<Field>();
        private readonly Int32 mIndex = 0;

        public string Name { get { return mName; } }
        public List<Field> Fields { get { return mFields; } }

        public Enum(Input input)
        {
            mName = input.GetNext();
            string pending = "";
            while (true)
            {
                string tag = "";
                if (pending.Equals(""))
                {
                    tag = input.GetNext();
                }
                else
                {
                    tag = pending;
                    pending = "";
                }
                if (tag.Equals("end")) return;
                string fieldName = tag;
                tag = input.GetNext();
                if (tag.Equals("="))
                {
                    mIndex = Int32.Parse(input.GetNext());
                }
                else
                {
                    pending = tag;
                }
                Fields.Add(new Field(fieldName, mIndex));
                mIndex++;
            }
        }
    }
}
