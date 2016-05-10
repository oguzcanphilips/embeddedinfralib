using System;

namespace codegen
{
    class Param
    {
        public enum Dir
        {
            In,
            Out,
            InOut
        };

        private readonly Dir mDir;
        private readonly string mType;
        private readonly string mName;

        public string Type { get { return mType; } }
        public string Name { get { return mName; } }
        public Dir Direction { get { return mDir; } }

        public Param(string t, string name)
        {
            mDir = Dir.In;
            if(name.Contains("(") ||
               name.Contains(")") ||
               name.Contains("[") ||
               name.Contains("]") ||
               name.Contains("<") ||
               name.Contains(">") ||
               name.Contains(",") ) throw new Exception("Invalid parameter name '"+name+"'");

            int dirStart = t.IndexOf('<');
            int dirEnd = t.IndexOf('>');
            if (dirStart > 0 && dirEnd > 0)
            {
                string direction = t.Substring(dirStart + 1, dirEnd - dirStart - 1);
                switch (direction.ToUpper())
                {
                    case "IN":
                        mDir = Dir.In;
                        break;
                    case "OUT":
                        mDir = Dir.Out;
                        break;
                    case "INOUT":
                        mDir = Dir.InOut;
                        break;
                    default:
                        throw  new Exception("Unknown direction : " + direction);
                }
                t = t.Substring(0, dirStart);
            }
            mName = name;
            mType = t;
        }
    }
}
