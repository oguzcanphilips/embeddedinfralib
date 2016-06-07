using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

namespace codegen
{
    partial class Parser
    {
        private string GetTypeCPP(Param p)
        {
            if (p.Direction != Param.Dir.In)
            {
                return p.Type + "&";
            }
            if (mSystemTypes.Contains(p.Type))
            {
                return p.Type;
            }
            return "const " + p.Type + "&";
        }

        public void GenerateCPP()
        {
            Directory.CreateDirectory(OutputFile.GetRoot() + "CPP");
            foreach (Interface i in mInterfaces)
            {
                GenerateCppInterfaceHFile(i);
                GenerateCppProxyHFile(i);
                GenerateCppProxyCFile(i);
                GenerateCppSkeletonHFile(i);
                GenerateCppSkeletonCFile(i);
            }
            GenereteCppLut();
            foreach (DataType d in mDataTypes)
            {
                GenerateCppDataTypeHFile(d);
                GenerateCppDataTypeCFile(d);
            }
            foreach (Enum en in mEnums)
            {
                GenerateCppEnumHFile(en);
            }
        }

        private void GenerateCppInterfaceHFile(Interface i)
        {
            string name = "I" + i.Name;
            using (StreamWriter file = new HeaderFile("CPP/" + name + ".hpp"))
            {
                file.WriteLine("#include \"PacketCommunication.hpp\"");
                if (i.HasEvents)
                {
                    file.WriteLine("#include \"infra\\util\\public\\SignalSlot.hpp\"");
                }
                AddIncludes(file, i.GetDependencies(), "#include \"{0}.hpp\"");
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.WriteLine("class " + name);
                file.WriteLine("{");
                file.WriteLine("public:");
                file.WriteLine("  virtual ~" + name + "(){}");
                file.WriteLine();
                List<Function> events = i.Events;
                foreach (var e in events)
                {
                    file.Write("  infra::Signal<" + name);
                    List<Param> ps = e.Params;
                    foreach (Param p in ps)
                    {
                        file.Write(", " + GetTypeCPP(p) + "/*"+p.Name+"*/");
                    }
                    file.WriteLine("> " + e.Name + "Signal;");
                }
                file.WriteLine();
                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.Write("  virtual " + f.Return + " " + f.Name + "(");
                    List<Param> ps = f.Params;
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeCPP(p) + " " + p.Name);
                    }
                    file.WriteLine(") = 0;");
                }
                file.WriteLine("protected:");
                foreach (var e in events)
                {
                    List<Param> ps = e.Params;
                    bool addComma = false;
                    string pars = "";
                    string args = "";
                    foreach (Param p in ps)
                    {
                        if (addComma) pars += ", ";
                        if (addComma) args += ", ";
                        addComma = true;
                        pars += GetTypeCPP(p) + " " + p.Name;
                        args += p.Name;
                    }
                    file.Write("  void " + e.Name + "("+pars+")");
                    file.WriteLine("{" + e.Name + "Signal(" + args + ");}");
                }
                file.WriteLine("};");
                file.WriteLine("}");
            }
        }

        private void GenerateCppProxyHFile(Interface i)
        {
            string name = i.Name + "Proxy";
            using (StreamWriter file = new HeaderFile("CPP/" + name + ".hpp"))
            {
                file.WriteLine("#include \"PacketCommunication.hpp\"");
                file.WriteLine("#include \"I" + i.Name + ".hpp\"");
                if (i.HasEvents)
                {
                    file.WriteLine("#include \"infra\\util\\public\\SignalSlot.hpp\"");
                    file.WriteLine("#include \"" + i.Name + "EventsSkeleton.hpp\"");
                }
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.Write("class " + name + " : public I" + i.Name); 
                if(i.HasEvents) file.Write(", private I"+i.Name+"Events");
                if(i.HasReturnValues) file.Write(", public PacketCommunication::Callback");
                file.WriteLine();
                file.WriteLine("{");
                file.WriteLine("public:");
                file.Write("  " + name + "(PacketCommunication& packetComm");
                file.WriteLine(");");
                if (i.HasReturnValues)
                {
                    file.WriteLine("  ~" + name + "();");
                    file.WriteLine("  void Receive(PacketCommunication& pc);");
                }
                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.Write("  " + f.Return + " " + f.Name + "(");
                    List<Param> ps = f.Params;
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeCPP(p) + " " + p.Name);
                    }
                    file.WriteLine(");");
                }
                file.WriteLine("private:");
                foreach (var e in i.Events)
                {
                    List<Param> ps = e.Params;
                    bool addComma = false;
                    string pars = "";
                    string args = "";
                    foreach (Param p in ps)
                    {
                        if (addComma) pars += ", ";
                        if (addComma) args += ", ";
                        addComma = true;
                        pars += GetTypeCPP(p) + " " + p.Name;
                        args += p.Name;
                    }
                    file.Write("  void " + e.Name + "(" + pars + ")");
                    file.WriteLine("{I"+i.Name+"::" + e.Name + "(" + args + ");}");
                }
                if (i.HasReturnValues)
                {
                    file.WriteLine("  volatile bool mReceiving;");
                    file.WriteLine("  volatile uint8_t mPendingFunctionId;");
                    file.WriteLine("  volatile uint8_t mPendingMessageId;");
                }
                file.WriteLine("  PacketCommunication& mPacketComm;");
                if (i.HasEvents)
                {
                    file.WriteLine("  " + i.Name + "EventsSkeleton mEventsSkeleton;");
                }
                file.WriteLine("};");
                file.WriteLine("}");
            }
        }

        private void GenerateCppProxyCFile(Interface i)
        {
            string name = i.Name + "Proxy";
            using (StreamWriter file = new OutputFile("CPP/" + name + ".cpp"))
            {
                file.WriteLine("#include \"" + name + ".hpp\"");
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.Write(name + "::" + name + "(PacketCommunication& packetComm");
                file.WriteLine(")");
                file.Write(": ");
                if (i.HasReturnValues)
                {
                    file.WriteLine("PacketCommunication::Callback(" + i.Id + ")");
                    file.WriteLine(", mReceiving(false)");
                    file.Write(", "); 
                }
                file.WriteLine("mPacketComm(packetComm)");
                file.WriteLine("{");
                if (i.HasReturnValues)
                {
                    file.WriteLine("  mPacketComm.Register(*this);");
                }
                if (i.HasEvents)
                {
                    file.WriteLine("  mPacketComm.Register(mEventsSkeleton);");
                    file.WriteLine("  mEventsSkeleton.SetImpl(*this);");
                }
                file.WriteLine("}");


                file.WriteLine();
                if (i.HasReturnValues)
                {
                    file.WriteLine(name + "::~" + name + "()");
                    file.WriteLine("{");
                    if (i.HasEvents)
                    {
                        file.WriteLine("  mPacketComm.Unregister(mEventsSkeleton);");
                    }
                    file.WriteLine("  mPacketComm.Unregister(*this);");
                    file.WriteLine("}");
                    file.WriteLine();
                    file.WriteLine("void " + name + "::Receive(PacketCommunication& pc)");
                    file.WriteLine("{");
                    file.WriteLine("  uint8_t functionId;");
                    file.WriteLine("  if(!pc.ReadFunctionId(functionId)) return;");
                    file.WriteLine("  if(mPendingFunctionId != functionId) return;");
                    file.WriteLine("  uint8_t messageId;");
                    file.WriteLine("  if(!pc.ReadMessageId(messageId)) return;");
                    file.WriteLine("  if(mPendingMessageId != messageId) return;");
                    file.WriteLine("  mReceiving = false;");
                    file.WriteLine("  pc.EventReceiveDoneReset();");
                    file.WriteLine("  pc.EventReceiveSet();");
                    file.WriteLine("  pc.EventReceiveDoneWait();");
                    file.WriteLine("}");
                    file.WriteLine();
                }
                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.WriteLine();
                    file.Write(f.Return + " " + name + "::" + f.Name + "(");
                    List<Param> ps = f.Params;
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeCPP(p) + " " + p.Name);
                    }
                    file.WriteLine(")");
                    file.WriteLine("{");
                    file.WriteLine("  PacketCommunication::FunctionScope functionScope(mPacketComm);");
                    file.WriteLine("  PacketCommunication::ReceiveScope receiveScope(mPacketComm);");
                    if (f.HasReturn)
                    {
                        file.WriteLine("  mPacketComm.EventReceiveReset();");
                        file.WriteLine("  mReceiving = true;");
                        file.WriteLine("  mPendingFunctionId = " + f.Id + ";");
                        file.WriteLine("  mPacketComm.PacketStart(" + i.Id + ", " + f.Id + ");");
                        file.WriteLine("  mPendingMessageId = mPacketComm.WriteMessageId();");
                    }
                    else
                    {
                        file.WriteLine("  mPacketComm.PacketStart(" + i.Id + ", " + f.Id + ");");
                    }
                    foreach (Param p in f.Params)
                    {
                        if (p.Direction != Param.Dir.Out)
                        {
                            file.WriteLine("  mPacketComm.Write(" + p.Name + ");");
                        }
                    }
                    file.WriteLine("  mPacketComm.PacketDone();");
                    if (f.HasReturn)
                    {
                        file.WriteLine("  mPacketComm.EventReceiveWait();");
                        file.WriteLine("  if(mReceiving) mPacketComm.ProcessReceive();");
                        
                        if (!f.ReturnVoid) file.WriteLine("  " + f.Return + " ret = " + f.Return + "();");
                        file.WriteLine("  if(mReceiving");
                        if (!f.ReturnVoid) file.WriteLine("    || !mPacketComm.Read(ret)");
                        foreach(Param p in f.Params)
                        {
                            if (p.Direction != Param.Dir.In)
                            {
                                file.WriteLine("    || !mPacketComm.Read("+p.Name+")");
                            }
                        }
                        file.WriteLine("    || !mPacketComm.ReadDone())");
                        file.WriteLine("  {");
                        file.WriteLine("    mPacketComm.HandleReceiveError();");
                        if (!f.ReturnVoid) file.WriteLine("    ret = " + f.Return + "();");
                        foreach (Param p in f.Params)
                        {
                            if (p.Direction != Param.Dir.In)
                            {
                                file.WriteLine("    " + p.Name + " = " + p.Type + "();");
                            }
                        }
                        file.WriteLine("  }");
                        if(!f.ReturnVoid) file.WriteLine("  return ret;");
                    }
                    file.WriteLine("}");
                }
                file.WriteLine("}");
            }
        }

        private void GenerateCppSkeletonHFile(Interface i)
        {
            string name = i.Name + "Skeleton";
            using (StreamWriter file = new HeaderFile("CPP/" + name + ".hpp"))
            {
                file.WriteLine("#include \"PacketCommunication.hpp\"");
                file.WriteLine("#include \"I" + i.Name + ".hpp\"");
                if (i.HasEvents)
                {
                    file.WriteLine("#include \"infra\\util\\public\\SignalSlot.hpp\"");
                    file.WriteLine("#include \"" + i.Name + "EventsProxy.hpp\"");
                }
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.WriteLine("class " + name + " : public PacketCommunication::Callback");
                file.WriteLine("{");
                file.WriteLine("public:");
                if (i.HasEvents)
                {
                    file.WriteLine("  " + name + "(PacketCommunication& packetCommEvents);");
                }
                else
                {
                    file.WriteLine("  " + name + "();");
                }
                file.WriteLine("  ~" + name + "();");
                file.WriteLine();
                if (i.HasEvents)
                {
                    List<Function> events = i.Events;
                    foreach (var e in events)
                    {
                        file.Write("  infra::Slot<" + i.Name + "EventsProxy");
                        List<Param> ps = e.Params;
                        foreach (Param p in ps)
                        {
                            file.Write(", " + GetTypeCPP(p) + "/*" + p.Name + "*/");
                        }
                        file.WriteLine("> " + e.Name + ";");
                    }
                    file.WriteLine();
                }
                file.WriteLine("  void SetImpl(I" + i.Name + "& impl);");
                file.WriteLine("  void Receive(PacketCommunication& pc);");
                file.WriteLine("private:");
                file.WriteLine("  I" + i.Name + "* mImpl;");
                if (i.HasEvents)
                {
                    file.WriteLine("  " + i.Name + "EventsProxy mEventsProxy;");
                }
                file.WriteLine("};");
                file.WriteLine("}");
            }
        }

        private void GenerateCppSkeletonCFile(Interface i)
        {
            string name = i.Name + "Skeleton";
            using (StreamWriter file = new OutputFile("CPP/" + name + ".cpp"))
            {
                file.WriteLine("#include \"" + name + ".hpp\"");
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                if (i.HasEvents)
                {
                    file.WriteLine(name + "::" + name + "(PacketCommunication& packetCommEvents)");
                    file.WriteLine(": Callback(" + i.Id + ") // Interface Id");
                    List<Function> events = i.Events;
                    foreach (var e in events)
                    {
                        file.WriteLine(", " + e.Name + "(&mEventsProxy, &" + i.Name + "EventsProxy::" + e.Name+")");
                    }
                    file.WriteLine(", mImpl(0)");
                    file.WriteLine(", mEventsProxy(packetCommEvents)");
                    file.WriteLine("{");
                    file.WriteLine("}");
                }
                else
                {
                    file.WriteLine(name + "::" + name + "()");
                    file.WriteLine(": Callback(" + i.Id + ") // Interface Id");
                    file.WriteLine(", mImpl(0)");
                    file.WriteLine("{");
                    file.WriteLine("}");
                }
                file.WriteLine();
                file.WriteLine(name + "::~" + name + "()");
                file.WriteLine("{");
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("void " + name + "::SetImpl(I" + i.Name + "& impl)");
                file.WriteLine("{");
                file.WriteLine("  mImpl = &impl;");
                List<Function> ev = i.Events;
                foreach (var e in ev)
                {
                    file.WriteLine("  mImpl->"+e.Name + "Signal += " + e.Name + ";");                    
                }
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("void " + name + "::Receive(PacketCommunication& pc)");
                file.WriteLine("{");
                file.WriteLine("  uint8_t func;");
                file.WriteLine("  if(!pc.ReadFunctionId(func)) return;");
                file.WriteLine("  switch(func)");
                file.WriteLine("  {");
                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.WriteLine("    case " + f.Id + ": // "+f.Name);
                    file.WriteLine("    {");
                    List<Param> ps = f.Params;
                    foreach (Param p in ps)
                    {
                        file.WriteLine("      " + p.Type + " " + p.Name + ";");
                    }
                    if (f.HasReturn)
                    {
                        file.WriteLine("      uint8_t messageId;");
                        file.WriteLine("      if(!pc.ReadMessageId(messageId)) return;");
                    }
                    foreach (Param p in ps)
                    {
                        if (p.Direction != Param.Dir.Out)
                        {
                            file.WriteLine("      if(!pc.Read(" + p.Name + ")) return;");
                        }
                    }
                    file.WriteLine("      if(!pc.ReadDone()) return;");

                    if (!f.ReturnVoid)
                    {
                        file.WriteLine("      " + f.Return + " ret = " + f.Return + "();");
                        file.Write("      if(mImpl) ret = ");
                    }
                    else
                    {
                        file.Write("      if(mImpl) ");
                    }
                    file.Write("mImpl->" + f.Name + "(");
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(p.Name);
                    }
                    file.WriteLine(");");
                    if (f.HasReturn)
                    {
                        file.WriteLine("      pc.PacketStart(" + i.Id + ", " + f.Id + ");");
                        file.WriteLine("      pc.WriteMessageId(messageId);");
                        if(!f.ReturnVoid) file.WriteLine("      pc.Write(ret);");
                        foreach (Param p in ps)
                        {
                            if (p.Direction != Param.Dir.In)
                            {
                                file.WriteLine("      pc.Write(" + p.Name + ");");
                            }
                        }
                        file.WriteLine("      pc.PacketDone();");
                    }
                    file.WriteLine("      break;");
                    file.WriteLine("    }");
                }
                file.WriteLine("    default:");
                file.WriteLine("      break;");
                file.WriteLine("  }");
                file.WriteLine("}");
                file.WriteLine("}");
            }
        }

        private void GenerateCppDataTypeHFile(DataType d)
        {
            using (StreamWriter file = new HeaderFile("CPP/" + d.Name + ".hpp"))
            {
                file.WriteLine("#include \"PacketCommunication.hpp\"");
                HashSet<string> deps = d.GetDependencies();
                foreach (string import in mImports)
                {
                    deps.Remove(import);
                    file.WriteLine("#include \"" + import + ".hpp\"");
                }
                foreach (string import in deps)
                {
                    file.WriteLine("#include \"" + import + ".hpp\"");
                }
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.WriteLine("class " + d.Name + ": public Serialize");
                file.WriteLine("{");
                file.WriteLine("public:");
                file.WriteLine("  " + d.Name + "();");
                file.WriteLine();
                file.WriteLine("  void Write(PacketCommunication& packetComm) const;");
                file.WriteLine("  bool Read(PacketCommunication& packetComm);");
                file.WriteLine();
                foreach (DataField p in d.Fields)
                {
                    string size = "";
                    if (p.Size > 1)
                    {
                        size = "[" + p.Size + "]";
                    }
                    file.WriteLine("  " + p.Type + " " + p.Name + size + ";");
                }
                file.WriteLine("};");
                file.WriteLine("}");
            }
        }

        private void GenerateCppDataTypeCFile(DataType d)
        {
            using (StreamWriter file = new OutputFile("CPP/" + d.Name + ".cpp"))
            {
                file.WriteLine("#include \"" + d.Name + ".hpp\"");
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                bool useComma = false;
                file.WriteLine(d.Name + "::" + d.Name + "()");
                foreach (DataField p in d.Fields)
                {
                    if (p.Size == 1)
                    {
                        if (useComma)
                        {
                            file.Write(", ");
                        }
                        else
                        {
                            file.Write(": ");
                            useComma = true;
                        }
                        file.WriteLine(p.Name + "(" + p.Type + "())");
                    }
                }
                file.WriteLine("{");
                foreach (DataField p in d.Fields)
                {
                    if (p.Size > 1)
                    {
                        file.WriteLine("  {");
                        file.WriteLine("    for(uint32_t i=0;i<" + p.Size + ";++i) " + p.Name + "[i] = " + p.Type + "();");
                        file.WriteLine("  }");
                    }
                }
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("void " + d.Name + "::Write(PacketCommunication& packetComm) const");
                file.WriteLine("{");
                foreach (DataField p in d.Fields)
                {
                    if (p.Size > 1)
                    {
                        file.WriteLine("  {");
                        file.WriteLine("    for(uint32_t i=0;i<" + p.Size + ";++i) packetComm.Write(" + p.Name + "[i]);");
                        file.WriteLine("  }");
                    }
                    else
                    {
                        file.WriteLine("  packetComm.Write(" + p.Name + ");");
                    }
                }
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("bool " + d.Name + "::Read(PacketCommunication& packetComm)");
                file.WriteLine("{");
                foreach (DataField p in d.Fields)
                {
                    if (p.Size > 1)
                    {
                        file.WriteLine("  {");
                        file.WriteLine("    for(uint32_t i=0;i<" + p.Size + ";++i) if(!packetComm.Read(" + p.Name + "[i])) return false;");
                        file.WriteLine("  }");
                    }
                    else
                    {
                        file.WriteLine("  if(!packetComm.Read(" + p.Name + ")) return false;");
                    }
                }
                file.WriteLine("  return true;");
                file.WriteLine("}");
                file.WriteLine("}");
            }
        }
        private void GenerateCppEnumHFile(Enum en)
        {
            using (StreamWriter file = new HeaderFile("CPP/" + en.Name + ".hpp"))
            {
                file.WriteLine("#include \"PacketCommunication.hpp\"");
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.WriteLine("class " + en.Name + " : public Serialize");
                file.WriteLine("{");
                file.WriteLine("public:");
                file.WriteLine("  enum Value");
                file.WriteLine("  {");

                bool writeComma = false;
                foreach (var f in en.Fields)
                {
                    if (writeComma)
                    {
                        file.WriteLine(",");
                    }
                    writeComma = true;
                    file.Write("    " + f.Name + " = " + f.Value);
                }
                file.WriteLine();
                file.WriteLine("  };");
                file.WriteLine("  " + en.Name + "() : value((Value)0){}");
                file.WriteLine("  " + en.Name + "(const Value& v) : value(v){}");
                file.WriteLine("  operator Value&(){return value;}");
                file.WriteLine("  operator Value() const {return value;}");
                file.WriteLine();
                file.WriteLine("  virtual void Write(PacketCommunication& pc) const");
                file.WriteLine("  {");
                file.WriteLine("    pc.Write((int32_t)value);");
                file.WriteLine("  }");
                file.WriteLine("  virtual bool Read(PacketCommunication& pc)");
                file.WriteLine("  {");
                file.WriteLine("      int32_t v;");
                file.WriteLine("      if (!pc.Read(v)) return false;");
                file.WriteLine("      value = (Value)v;");
                file.WriteLine("      return true;");
                file.WriteLine("  }");
                file.WriteLine("private:");
                file.WriteLine("  Value value;");
                file.WriteLine("};");
                file.WriteLine("}");
            }
        }

        private void GenereteCppLut()
        {
            using (StreamWriter file = new HeaderFile("CPP/PacketCommunicationLut.hpp"))
            {
                file.WriteLine("#include <cstdint>");
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.WriteLine("  namespace Lut");
                file.WriteLine("  {");
                file.WriteLine("    struct FunctionId");
                file.WriteLine("    {");
                file.WriteLine("      const char* name;");
                file.WriteLine("      const uint8_t id;");
                file.WriteLine("    };");
                file.WriteLine("    struct InterfaceSpec");
                file.WriteLine("    {");
                file.WriteLine("      const char* name;");
                file.WriteLine("      const uint8_t id;");
                file.WriteLine("      const uint8_t numberOfFuncions;");
                file.WriteLine("      const uint16_t functionIndex;");
                file.WriteLine("    };");
                file.WriteLine("    struct EnumField");
                file.WriteLine("    {");
                file.WriteLine("      const char* name;");
                file.WriteLine("      const int32_t id;");
                file.WriteLine("    };");
                file.WriteLine("    struct EnumSpec");
                file.WriteLine("    {");
                file.WriteLine("      const char* name;");
                file.WriteLine("      const uint8_t numberOfFields;");
                file.WriteLine("      const uint16_t fieldIndex;");
                file.WriteLine("    };");


                string functions = "    const static struct FunctionId functionIds[] =" + Environment.NewLine;
                functions += "    {" + Environment.NewLine;

                string interfaceSpecs = "    const static struct InterfaceSpec interfaceSpecs[] =" +  Environment.NewLine;
                interfaceSpecs += "    {"+Environment.NewLine;

                string enumFields = "    const static struct EnumField enumFields[] =" + Environment.NewLine;
                enumFields += "    {" + Environment.NewLine;

                string enumSpecs = "    const static struct EnumSpec enumSpecs[] =" + Environment.NewLine;
                enumSpecs += "    {" + Environment.NewLine;

                int nameIndex = 0;
                foreach (var interfaceSpec in mInterfaces)
                {
                    interfaceSpecs += "      {\"" + interfaceSpec.Name + "\", " + interfaceSpec.Id + ", " +
                                      interfaceSpec.Functions.Count + ", " + nameIndex + "}," + Environment.NewLine;

                    nameIndex += interfaceSpec.Functions.Count;
                    foreach (var func in interfaceSpec.Functions)
                    {
                        functions += "      {\"" + func.Name + "\", " + func.Id + "}," + Environment.NewLine;
                    }
                }
                nameIndex = 0;
                foreach (var enumSpec in mEnums)
                {
                    enumSpecs += "      {\"" + enumSpec.Name + "\", " +
                                      enumSpec.Fields.Count + ", " + nameIndex + "}," + Environment.NewLine;

                    nameIndex += enumSpec.Fields.Count;
                    foreach (var eSpec in enumSpec.Fields)
                    {
                        enumFields += "      {\"" + eSpec.Name + "\", " + eSpec.Value + "}," + Environment.NewLine;
                    }
                }

                functions += "    };";
                enumFields += "    };";
                interfaceSpecs += "    };";
                enumSpecs += "    };";
                file.WriteLine(functions);
                file.WriteLine(interfaceSpecs);
                file.WriteLine(enumFields);
                file.WriteLine(enumSpecs);
                file.WriteLine("  }");
                file.WriteLine("}");
            }            
        }
    }
}

