using System;
using System.Collections.Generic;
using System.IO;

namespace codegen
{
    partial class Parser
    {
        private readonly HashSet<string> mImports = new HashSet<string>();
        private readonly Dictionary<string, string> mCAliases = new Dictionary<string, string>();
        private readonly List<DataType> mDataTypes = new List<DataType>();
        private readonly List<Enum> mEnums = new List<Enum>();
        private readonly List<Interface> mInterfaces = new List<Interface>();
        static internal string[] mSystemTypes = new String[] { "void", "bool", "uint8_t", "uint16_t", "uint32_t", "int8_t", "int16_t", "int32_t" };

        public Parser(Input inp)
        {
            while (!inp.Empty)
            {
                string tag = inp.GetNext();
                switch (tag)
                {
                    case "data":
                        mDataTypes.Add(new DataType(inp));
                        break;
                    case "enum":
                        mEnums.Add(new Enum(inp));
                        break;
                    case "interface":
                        var i = new Interface(inp);
                        AddInterface(i);
                        AddInterface(i.CreateEventInterface());
                        break;
                    case "import":
                        mImports.Add(inp.GetNext());
                        break;
                    case "c_alias":
                        string from = inp.GetNext();
                        string to = inp.GetNext();
                        mCAliases.Add(from, to);
                        break;
                    default:
                        if (tag.Length > 0)
                        {
                            throw new Exception("Unknown Tag : (" + tag + ")");
                        }
                        break;
                }
            }
            GenerateCode();
        }

        private void GenerateCode()
        {
            GenerateMCPP();
            GenerateCPP();
            GenerateC();
        }

        private void AddInterface(Interface i)
        {
            if (i != null)
            {
                Console.WriteLine("Interface : " + i.Name);
                foreach(Interface it in mInterfaces)
                    if(it.Id == i.Id)
                        throw new Exception("Duplicate interface id: " + i.Id/2);
                mInterfaces.Add(i);
            }
        }

        void AddIncludes(StreamWriter file, HashSet<string> deps, string includeFormat)
        {
            foreach (string type in mSystemTypes)
            {
                deps.Remove(type);
            }
            foreach (string import in mImports)
            {
                deps.Add(import);
            }
            foreach (string dep in deps)
            {
                file.WriteLine(String.Format(includeFormat, dep));
            }
        }
    }
}
