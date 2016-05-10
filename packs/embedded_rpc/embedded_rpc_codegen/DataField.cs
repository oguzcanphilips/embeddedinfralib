using System;

namespace codegen
{
    class DataField
    {
        private readonly string mType;
        private readonly string mName;
        private readonly UInt32 mSize;

        public string Type { get { return mType; } }
        public string Name { get { return mName; } }
        public UInt32 Size { get { return mSize; } }

        public DataField(string t, string name)
        {
            mType = t;

            if (name.Contains("(") ||
               name.Contains(")") ||
               name.Contains("<") ||
               name.Contains(">") ||
               name.Contains(",")) throw new Exception("Invalid data field name '" + name + "'");

            int arrayStart = name.IndexOf('[');
            int arrayEnd = name.IndexOf(']');
            if (arrayStart > 0 && arrayEnd > 0)
            {
                string size = name.Substring(arrayStart + 1, arrayEnd - arrayStart - 1);
                mSize = UInt32.Parse(size);
                mName = name.Substring(0, arrayStart);
            }
            else
            {
                mName = name;
                mSize = 1;
            }
        }
    }
}
