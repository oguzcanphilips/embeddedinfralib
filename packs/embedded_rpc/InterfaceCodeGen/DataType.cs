using System.Collections.Generic;

namespace InterfaceCodeGen
{
    class DataType
    {
        private readonly string mName;
        private readonly List<DataField> mFields = new List<DataField>();
        public string Name { get { return mName; } }
        public List<DataField> Fields { get { return mFields; } }

        public DataType(Input input)
        {
            mName = input.GetNext();

            while (true)
            {
                string tag = input.GetNext();
                if (tag.Equals("end")) return;
                string type = tag;
                string name = input.GetNext();
                Fields.Add(new DataField(type, name));
            }
        }
        public HashSet<string> GetDependencies()
        {
            HashSet<string> res = new HashSet<string>();
            foreach (var param in mFields)
            {
                res.Add(param.Type);
            }
            foreach (var t in Parser.mSystemTypes)
            {
                res.Remove(t);
            }
            return res;
        }

    }
}
