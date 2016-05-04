using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

namespace InterfaceCodeGen
{
    partial class Parser
    {
        private string GetTypeC(string type)
        {
            if (mCAliases.ContainsKey(type))
            {
                type = mCAliases[type];
            }
            return type;
        }

        private string GetTypeCParam(string type)
        {
            if (!mSystemTypes.Contains(type))
            {
                type = GetTypeC(type) + "*";
            }
            return type;
        }

        private string GetTypeCParam(Param p)
        {
            string type = GetTypeC(p.Type);
            if (!mSystemTypes.Contains(type) || p.Direction != Param.Dir.In)
            {
                return type + "*";
            }
            return type;
        }

        public void GenerateC()
        {
            Directory.CreateDirectory(OutputFile.GetRoot() + "C");
            foreach (Interface i in mInterfaces)
            {
                GenerateCHFile(i);
                GenerateCProxyCFile(i);
                GenerateCSkeletonHFile(i);
                GenerateCSkeletonCFile(i);
            }
            foreach (DataType d in mDataTypes)
            {
                GenerateCDataTypeHFile(d);
                GenerateCDataTypeCFile(d);
            }
            foreach (Enum en in mEnums)
            {
                GenerateCEnumHFile(en);
            }
        }

        private void GenerateCHFile(Interface i)
        {
            string name = i.Name;
            using (StreamWriter file = new HeaderFile("C/" + name + ".h"))
            {
                file.WriteLine("#include \"PacketCommunication.h\"");
                AddIncludes(file, i.GetDependencies(), "#include \"{0}.h\"");
                file.WriteLine();
                file.WriteLine("/* These functions are only valid for the proxy */");
                file.WriteLine("void " + name + "_Init(PacketCommunicationDefinition_t* pc);");
                file.WriteLine("void " + name + "_DeInit(PacketCommunicationDefinition_t* pc);");
                file.WriteLine();
                file.WriteLine();
                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.Write(GetTypeC(f.Return) + " " + name + "_" + f.Name + "(");
                    List<Param> ps = f.Params;
                    if (ps.Count == 0)
                    {
                        file.Write("void");
                    }
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeCParam(p) + " " + p.Name);
                    }
                    file.WriteLine(");");
                }
            }
        }

        private void GenerateCProxyCFile(Interface i)
        {
            string name = i.Name;
            using (StreamWriter file = new OutputFile("C/" + name + "Proxy.c"))
            {
                file.WriteLine("#include \"" + name + ".h\"");
                file.WriteLine("#include <string.h>");
                file.WriteLine();
                file.WriteLine("static PacketCommunicationDefinition_t* pcomm = 0;");
                if (i.HasReturnValues)
                {
                    file.WriteLine("static volatile uint8_t receiving = false;");
                    file.WriteLine("static volatile uint8_t pendingMessageCnt = 0;");
                    file.WriteLine("static volatile uint8_t pendingFunctionId = 0;");
                    file.WriteLine("static uint8_t messageCntGen = 0;");
                    file.WriteLine("static void Receive(PacketCommunicationDefinition_t* pc);");
                    file.WriteLine("static packetCommunicationCallback_t callback = {" + i.Id + ", 0, 0, &Receive};");
                    file.WriteLine();
                    file.WriteLine("static void Receive(PacketCommunicationDefinition_t* pc)");
                    file.WriteLine("{");
                    file.WriteLine("  uint8_t messageCnt;");
                    file.WriteLine("  uint8_t functionId;");
                    file.WriteLine("  if(!PacketCommunication_Read_uint8_t(pc, &functionId)) return;");
                    file.WriteLine("  if(pendingFunctionId != functionId) return;");
                    file.WriteLine("  if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;");
                    file.WriteLine("  if(pendingMessageCnt != messageCnt) return;");
                    file.WriteLine("  receiving = false;");
                    file.WriteLine("}");
                    file.WriteLine();
                }
                file.WriteLine("void " + name + "_Init(PacketCommunicationDefinition_t* pc)");
                file.WriteLine("{");
                file.WriteLine("  if(pcomm) return;");
                file.WriteLine("  pcomm = pc;");
                if (i.HasReturnValues)
                {
                    file.WriteLine("  PacketCommunication_Register(pcomm, &callback);");
                }
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("void " + name + "_DeInit(PacketCommunicationDefinition_t* pc)");
                file.WriteLine("{");
                file.WriteLine("  if(pcomm != pc) return;");
                if (i.HasReturnValues)
                {
                    file.WriteLine("  PacketCommunication_Unregister(pcomm, &callback);");
                }
                file.WriteLine("  pcomm = 0;");
                file.WriteLine("}");
                    
                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.WriteLine();
                    file.Write(GetTypeC(f.Return) + " " + name + "_" + f.Name + "(");
                    List<Param> ps = f.Params;
                    if (ps.Count == 0)
                    {
                        file.Write("void");
                    }
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeCParam(p) + " " + p.Name);
                    }
                    file.WriteLine(")");
                    file.WriteLine("{");
                    if (f.HasReturn)
                    {
                        file.WriteLine("  receiving = true;");
                        file.WriteLine("  pendingFunctionId = " + f.Id + ";");
                        file.WriteLine("  pendingMessageCnt = messageCntGen++;");
                        file.WriteLine("  PacketCommunication_PacketStart_InterfaceFunction(pcomm, " + (i.Id + 128) + ", " + f.Id + ");");
                        file.WriteLine("  PacketCommunication_Write_uint8_t(pcomm, pendingMessageCnt);");
                    }
                    else
                    {
                        file.WriteLine("  PacketCommunication_PacketStart_InterfaceFunction(pcomm, " + (i.Id + 128) + ", " + f.Id + ");");
                    }
                    foreach (Param p in f.Params)
                    {
                        if (p.Direction == Param.Dir.In)
                        {
                            file.WriteLine("  PacketCommunication_Write_" + p.Type + "(pcomm, " + p.Name + ");");
                        }
                        else if (p.Direction == Param.Dir.InOut)
                        {
                            file.WriteLine("  PacketCommunication_Write_" + p.Type + "(pcomm, *" + p.Name + ");");
                        }
                    }
                    file.WriteLine("  PacketCommunication_PacketDone_InterfaceFunction(pcomm);");
                    if (f.HasReturn)
                    {
                        file.WriteLine("  if(receiving) pcomm->ProcessReceive(pcomm);");
                        file.WriteLine();
                        file.WriteLine("  {");
                        if (!f.ReturnVoid) file.WriteLine("    " + GetTypeC(f.Return) + " ret;");
                        file.WriteLine("    if(receiving");
                        if (!f.ReturnVoid) file.WriteLine("      || !PacketCommunication_Read_" + f.Return + "(pcomm, &ret)");
                        foreach (Param p in f.Params)
                        {
                            if (p.Direction != Param.Dir.In)
                            {
                                file.WriteLine("      || !PacketCommunication_Read_" + p.Type + "(pcomm, " + p.Name + ")");
                            }
                        }
                        file.WriteLine("      || !PacketCommunication_ReadDone(pcomm))");
                        file.WriteLine("    {");
                        if (!f.ReturnVoid) file.WriteLine("      memset((void*)&ret, 0, sizeof(ret));");
                        foreach (Param p in f.Params)
                        {
                            if (p.Direction != Param.Dir.In)
                            {
                                file.WriteLine("      memset((void*)" + p.Name + ",0,sizeof(*"+p.Name+"));");
                            }
                        }
                        file.WriteLine("      pcomm->HandleReceiveError(pcomm);");
                        file.WriteLine("    }");
                        if (!f.ReturnVoid) file.WriteLine("    return ret;");
                        file.WriteLine("  }");
                    }
                    file.WriteLine("}");
                }
            }
        }

        private void GenerateCSkeletonHFile(Interface i)
        {
            string name = i.Name + "Skeleton";
            using (StreamWriter file = new HeaderFile("C/" + name + ".h"))
            {
                file.WriteLine("#include \"PacketCommunication.h\"");
                file.WriteLine("void " + name + "_Init(PacketCommunicationDefinition_t* pc);");
                file.WriteLine("void " + name + "_DeInit(PacketCommunicationDefinition_t* pc);");
            }
        }

        private void GenerateCSkeletonCFile(Interface i)
        {
            string name = i.Name + "Skeleton";
            using (StreamWriter file = new OutputFile("C/" + name + ".c"))
            {
                file.WriteLine("#include \"" + name + ".h\"");
                file.WriteLine("#include \"" + i.Name + ".h\"");
                file.WriteLine();
                file.WriteLine("static void Receive(PacketCommunicationDefinition_t* pc);");
                file.WriteLine("static packetCommunicationCallback_t callback = {" + (i.Id + 128) + ", 0, 0, &Receive};");
                file.WriteLine();
                file.WriteLine();
                file.WriteLine("void " + name + "_Init(PacketCommunicationDefinition_t* pc)");
                file.WriteLine("{");
                file.WriteLine("  PacketCommunication_Register(pc, &callback);");
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("void " + name + "_DeInit(PacketCommunicationDefinition_t* pc)");
                file.WriteLine("{");
                file.WriteLine("  PacketCommunication_Unregister(pc, &callback);");
                file.WriteLine("}");
                file.WriteLine();


                file.WriteLine("static void Receive(PacketCommunicationDefinition_t* pc)");
                file.WriteLine("{");
                file.WriteLine("  uint8_t func;");
                file.WriteLine("  if(!PacketCommunication_Read_uint8_t(pc, &func)) return;");
                file.WriteLine("  switch(func)");
                file.WriteLine("  {");
                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.WriteLine("    case " + f.Id + ":");
                    file.WriteLine("    {");
                    List<Param> ps = f.Params;
                    foreach (Param p in ps)
                    {
                        file.WriteLine("      " + GetTypeC(p.Type) + " " + p.Name + ";");
                    }
                    if (f.HasReturn)
                    {
                        file.WriteLine("      uint8_t messageCnt;");
                        file.WriteLine("      if(!PacketCommunication_Read_uint8_t(pc, &messageCnt)) return;");
                    }
                    foreach (Param p in ps)
                    {
                        if (p.Direction != Param.Dir.Out)
                        {
                            file.WriteLine("      if(!PacketCommunication_Read_" + p.Type + "(pc, &" + p.Name + ")) return;");
                        }
                    }
                    file.WriteLine("      if(!PacketCommunication_ReadDone(pc)) return;");

                    file.WriteLine("      {");
                    if (!f.ReturnVoid)
                    {
                        file.Write("        " + GetTypeC(f.Return) + " ret = ");
                    }
                    else
                    {
                        file.Write("        ");
                    }
                    file.Write(i.Name + "_" + f.Name + "(");
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        if (i.GetDependencies().Contains(p.Type) || p.Direction != Param.Dir.In)
                        {
                            file.Write("&"); // use pointers for custom imports and outputs
                        }
                        file.Write(p.Name);
                    }
                    file.WriteLine(");");
                    if (f.HasReturn)
                    {
                        file.WriteLine("        PacketCommunication_PacketStart_InterfaceFunction(pc, " + i.Id + ", " + f.Id + ");");
                        file.WriteLine("        PacketCommunication_Write_uint8_t(pc, messageCnt);");
                        if (!f.ReturnVoid)
                        {
                            file.Write("        PacketCommunication_Write_" + f.Return + "(pc, ");
                            if (i.GetDependencies().Contains(f.Return) || mCAliases.ContainsKey(f.Return))
                            {
                                file.Write("&"); // use pointers for custom imports
                            }
                            file.WriteLine("ret);");
                        }
                        // add output params.
                        foreach (Param p in ps)
                        {
                            if (p.Direction != Param.Dir.In)
                            {
                                file.Write("        PacketCommunication_Write_" + p.Type + "(pc, " + p.Name + ");");
                            }
                        }
                        file.WriteLine("        PacketCommunication_PacketDone_InterfaceFunction(pc);");
                    }                
                    file.WriteLine("      };");
                    file.WriteLine("      break;");
                    file.WriteLine("    }");
                }
                file.WriteLine("    default:");
                file.WriteLine("      break;");
                file.WriteLine("  }");
                file.WriteLine("}");
            }
        }

        private void GenerateCDataTypeHFile(DataType d)
        {
            using (StreamWriter file = new HeaderFile("C/" + d.Name + ".h"))
            {
                file.WriteLine("#include \"PacketCommunication.h\"");
                HashSet<string> deps = d.GetDependencies();
                foreach (string import in mImports)
                {
                    deps.Remove(import);
                    file.WriteLine("#include \"" + import + ".h\"");
                }
                foreach (string import in deps)
                {
                    file.WriteLine("#include \"" + import + ".h\"");
                }
                file.WriteLine();
                file.WriteLine("typedef struct");
                file.WriteLine("{");
                foreach (DataField p in d.Fields)
                {
                    string size = "";
                    if (p.Size > 1)
                    {
                        size = "[" + p.Size + "]";
                    }
                    file.WriteLine("  " + GetTypeC(p.Type) + " " + p.Name + size + ";");
                }
                file.WriteLine("} " + GetTypeC(d.Name) + ";");
                file.WriteLine();
                file.WriteLine("void PacketCommunication_Write_" + d.Name + "(PacketCommunicationDefinition_t* pc, " + GetTypeCParam(d.Name) + " value);");
                file.WriteLine("bool PacketCommunication_Read_" + d.Name + "(PacketCommunicationDefinition_t* pc, " + GetTypeCParam(d.Name) + " value);");
            }
        }

        private void GenerateCDataTypeCFile(DataType d)
        {
            using (StreamWriter file = new OutputFile("C/" + d.Name + ".c"))
            {
                file.WriteLine("#include \"" + d.Name + ".h\"");
                file.WriteLine();
                file.WriteLine("void PacketCommunication_Write_" + d.Name + "(PacketCommunicationDefinition_t* pc, " + GetTypeCParam(d.Name) + " value)");
                file.WriteLine("{");
                foreach (DataField p in d.Fields)
                {
                    if (p.Size > 1)
                    {
                        file.WriteLine("  {");
                        file.WriteLine("    uint32_t i;");
                        if(mSystemTypes.Contains(p.Type))
                        {
                            file.WriteLine("    for(i=0;i<" + p.Size + ";++i) PacketCommunication_Write_" + p.Type + "(pc, value->" + p.Name + "[i]);");
                        }
                        else
                        {
                            file.WriteLine("    for(i=0;i<" + p.Size + ";++i) PacketCommunication_Write_" + p.Type + "(pc, &value->" + p.Name + "[i]);");
                        }
                        file.WriteLine("  }");
                    }
                    else
                    {
                        if (mSystemTypes.Contains(p.Type))
                        {
                            file.WriteLine("  PacketCommunication_Write_" + p.Type + "(pc, value->" + p.Name + ");");
                        }
                        else
                        {
                            file.WriteLine("  PacketCommunication_Write_" + p.Type + "(pc, &value->" + p.Name + ");");
                        }
                    }
                }
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("bool PacketCommunication_Read_" + d.Name + "(PacketCommunicationDefinition_t* pc, " + GetTypeCParam(d.Name) + " value)");
                file.WriteLine("{");
                foreach (DataField p in d.Fields)
                {
                    if (p.Size > 1)
                    {
                        file.WriteLine("  {");
                        file.WriteLine("    uint32_t i;");
                        file.WriteLine("    for(i=0;i<" + p.Size + ";++i) if(!PacketCommunication_Read_" + p.Type + "(pc, &value->" + p.Name + "[i])) return false;");
                        file.WriteLine("  }");
                    }
                    else
                    {
                        file.WriteLine("  if(!PacketCommunication_Read_" + p.Type + "(pc, &value->" + p.Name + ")) return false;");
                    }
                }
                file.WriteLine("  return true;");
                file.WriteLine("}");
            }
        }

        private void GenerateCEnumHFile(Enum en)
        {
            using (StreamWriter file = new HeaderFile("C/" + en.Name + ".h"))
            {
                file.WriteLine("#include \"PacketCommunication.h\"");
                file.WriteLine();
                file.WriteLine("typedef enum");
                file.WriteLine("{");
                bool writeComma = false;
                foreach (var f in en.Fields)
                {
                    if (writeComma)
                    {
                        file.WriteLine(",");
                    }
                    writeComma = true;
                    file.Write("  " + en.Name + "_" + f.Name + " = " + f.Value);
                }
                file.WriteLine();
                file.WriteLine("} " + GetTypeC(en.Name) + ";");
                file.WriteLine();
                file.WriteLine("static inline void PacketCommunication_Write_" + en.Name + "(PacketCommunicationDefinition_t* pc, " + GetTypeCParam(en.Name) + " value)");
                file.WriteLine("{");
                file.WriteLine("  PacketCommunication_Write_int32_t(pc, (int32_t)*value);");
                file.WriteLine("}");
                file.WriteLine("static inline bool PacketCommunication_Read_" + en.Name + "(PacketCommunicationDefinition_t* pc, " + GetTypeCParam(en.Name) + " value)");
                file.WriteLine("{");
                file.WriteLine("  int32_t tmp;");
                file.WriteLine("  if(!PacketCommunication_Read_int32_t(pc, &tmp)) return false;");
                file.WriteLine("  *value = (" + GetTypeC(en.Name) + ")tmp;");
                file.WriteLine("  return true;");
                file.WriteLine("}");
            }
        }
    }
}