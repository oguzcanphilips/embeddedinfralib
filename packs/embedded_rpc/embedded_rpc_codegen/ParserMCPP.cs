using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

namespace codegen
{
    partial class Parser
    {
        private string GetTypeMCPP(Param p)
        {
            if (mSystemTypes.Contains(p.Type))
            {
                if (p.Direction != Param.Dir.In)
                {
                    return p.Type + "%";
                }
                return p.Type;
            }
            return p.Type + "^";
        }

        private string GetTypeMCPP(string type)
        {
            if (mSystemTypes.Contains(type))
            {
                return type;
            }
            return type + "^";
        }

        private string GetInstanceMCPP(string type)
        {
            if (mSystemTypes.Contains(type))
            {
                return type + "()";
            }
            return "gcnew " + type + "()";
        }

        private string GetTypeInstanceMCPP(string type, string instance)
        {
            return GetTypeMCPP(type) + " " + instance + " = " + GetInstanceMCPP(type) + ";";
        }


        public void GenerateMCPP()
        {
            Directory.CreateDirectory(OutputFile.GetRoot() + "MCPP");
            foreach (Interface i in mInterfaces)
            {
                GenerateMCppInterfaceHFile(i);
                GenerateMCppProxyHFile(i);
                GenerateMCppProxyCFile(i);
                GenerateMCppSkeletonHFile(i);
                GenerateMCppSkeletonCFile(i);
            }
            foreach (DataType d in mDataTypes)
            {
                GenerateMCppDataTypeHFile(d);
                GenerateMCppDataTypeCFile(d);
            }
            foreach (Enum en in mEnums)
            {
                GenerateMCppEnumHFile(en);
            }
        }

        private void GenerateMCppInterfaceHFile(Interface i)
        {
            string name = "I" + i.Name;
            using (StreamWriter file = new OutputFile("MCPP/" + name + ".hpp"))
            {
                file.WriteLine(@"#pragma once");
                file.WriteLine();
                file.WriteLine("#include \"PacketCommunication.hpp\"");
                AddIncludes(file, i.GetDependencies(), "#include \"{0}.hpp\"");
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.WriteLine("public interface class " + name);
                file.WriteLine("{");
                file.WriteLine("public:");
                List<Function> ev = i.Events;
                foreach (Function f in ev)
                {
                    file.Write("  delegate void " + f.Name + "Delegate(");
                    List<Param> ps = f.Params;
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeMCPP(p) + " " + p.Name);
                    }
                    file.WriteLine(");");
                    file.WriteLine("  event " + f.Name + "Delegate^ " + f.Name + "Event;");
                }
                file.WriteLine();

                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.Write("  " + GetTypeMCPP(f.Return) + " " + f.Name + "(");
                    List<Param> ps = f.Params;
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeMCPP(p) + " " + p.Name);
                    }
                    file.WriteLine(");");
                }
                file.WriteLine("};");
                file.WriteLine("}");
            }
        }

        private void GenerateMCppProxyHFile(Interface i)
        {
            string name = i.Name + "Proxy";
            using (StreamWriter file = new OutputFile("MCPP/" + name + ".hpp"))
            {
                file.WriteLine(@"#pragma once");
                file.WriteLine();
                file.WriteLine("#include \"PacketCommunication.hpp\"");
                file.WriteLine("#include \"I" + i.Name + ".hpp\"");
                if (i.HasEvents)
                {
                    file.WriteLine("#include \"" + i.Name + "EventsSkeleton.hpp\"");
                }
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.Write("public ref class " + name + " : public I" + i.Name);
                if (i.HasEvents) file.Write(", public I" + i.Name + "Events");
                if (i.HasReturnValues) file.Write(", public PacketCommunication::Callback");
                file.WriteLine();
                file.WriteLine("{");
                file.WriteLine("public:");
                List<Function> ev = i.Events;
                foreach (Function f in ev)
                {
                    file.WriteLine("  virtual event I" + i.Name + "::" + f.Name + "Delegate^ " + f.Name + "Event;");
                    file.Write("  virtual void " + f.Name + "(");
                    List<Param> ps = f.Params;
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeMCPP(p) + " " + p.Name);
                    }
                    file.Write("){ " + f.Name + "Event(");
                    addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(p.Name);
                    }
                    file.WriteLine(");}");
                }
                file.WriteLine();
                file.Write("  " + name + "(PacketCommunication^ packetComm");
                file.WriteLine(");");
                file.WriteLine("  ~" + name + "();");
                if (i.HasReturnValues)
                {
                    file.WriteLine("  virtual void Receive(PacketCommunication^ pc) override;");
                }
                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.Write("  virtual " + GetTypeMCPP(f.Return) + " " + f.Name + "(");
                    List<Param> ps = f.Params;
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeMCPP(p) + " " + p.Name);
                    }
                    file.WriteLine(");");
                }
                file.WriteLine("private:");
                if (i.HasReturnValues)
                {
                    file.WriteLine("  volatile bool mReceiving;");
                    file.WriteLine("  volatile uint8_t mMessageCnt;");
                    file.WriteLine("  volatile uint8_t mPendingFunctionId;");
                    file.WriteLine("  volatile uint8_t mPendingMessageCnt;");
                }
                file.WriteLine("  PacketCommunication^ mPacketComm;");
                if (i.HasEvents)
                {
                    file.WriteLine("  " + i.Name + "EventsSkeleton^ mEventsSkeleton;");
                }
                file.WriteLine("};");
                file.WriteLine("}");
            }
        }

        private void GenerateMCppProxyCFile(Interface i)
        {
            string name = i.Name + "Proxy";
            using (StreamWriter file = new OutputFile("MCPP/" + name + ".cpp"))
            {
                file.WriteLine("#include \"" + name + ".hpp\"");
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.Write(name + "::" + name + "(PacketCommunication^ packetComm");
                file.WriteLine(")");
                file.Write(": ");
                if (i.HasReturnValues)
                {
                    file.WriteLine("PacketCommunication::Callback(" + i.Id + ")");
                    file.WriteLine(", mReceiving(false)");
                    file.WriteLine(", mMessageCnt(0)");
                    file.Write(", ");
                }
                file.WriteLine("mPacketComm(packetComm)");
                if (i.HasEvents)
                {
                    file.WriteLine(", mEventsSkeleton(gcnew " + i.Name + "EventsSkeleton(packetComm))");
                }
                file.WriteLine("{");
                if (i.HasReturnValues)
                {
                    file.WriteLine("  mPacketComm->Register(this);");
                }
                if (i.HasEvents)
                {
                    file.WriteLine("  mEventsSkeleton->SetImpl(this);");
                }
                file.WriteLine("}");


                file.WriteLine();
                file.Write(name + "::~" + name + "()");
                file.WriteLine("{");
                if (i.HasReturnValues)
                {
                    file.WriteLine("  mPacketComm->Unregister(this);");
                }
                if (i.HasEvents)
                {
                    file.WriteLine("  mEventsSkeleton->~" + i.Name + "EventsSkeleton();");
                }
                file.WriteLine("}");
                if (i.HasReturnValues)
                {
                    file.WriteLine();
                    file.WriteLine("void " + name + "::Receive(PacketCommunication^ pc)");
                    file.WriteLine("{");
                    file.WriteLine("  uint8_t functionId;");
                    file.WriteLine("  if(!pc->Read(functionId)) return;");
                    file.WriteLine("  if(mPendingFunctionId != functionId) return;");
                    file.WriteLine("  uint8_t messageCnt;");
                    file.WriteLine("  if(!pc->Read(messageCnt)) return;");
                    file.WriteLine("  if(mPendingMessageCnt != messageCnt) return;");
                    file.WriteLine("  mReceiving = false;");
                    file.WriteLine("  pc->EventReceiveDoneReset();");
                    file.WriteLine("  pc->EventReceiveSet();");
                    file.WriteLine("  pc->EventReceiveDoneWait();");
                    file.WriteLine("}");
                    file.WriteLine();
                }
                List<Function> fs = i.Functions;
                foreach (Function f in fs)
                {
                    file.WriteLine();
                    file.Write(GetTypeMCPP(f.Return) + " " + name + "::" + f.Name + "(");
                    List<Param> ps = f.Params;
                    bool addComma = false;
                    foreach (Param p in ps)
                    {
                        if (addComma) file.Write(", ");
                        addComma = true;
                        file.Write(GetTypeMCPP(p) + " " + p.Name);
                    }
                    file.WriteLine(")");
                    file.WriteLine("{");
                    file.WriteLine("  PacketCommunication::FunctionScope functionScope(mPacketComm);");
                    if (f.HasReturn)
                    {
                        file.WriteLine("  PacketCommunication::ReceiveScope receiveScope(mPacketComm);");
                        file.WriteLine("  mReceiving = true;");
                        file.WriteLine("  mPendingFunctionId = " + f.Id + ";");
                        file.WriteLine("  mPendingMessageCnt = mMessageCnt++;");
                        file.WriteLine("  mPacketComm->PacketStart(" + (i.Id + 128) + ", " + f.Id + ");");
                        file.WriteLine("  mPacketComm->Write(mPendingMessageCnt);");
                    }
                    else
                    {
                        file.WriteLine("  mPacketComm->PacketStart(" + (i.Id + 128) + ", " + f.Id + ");");
                    }
                    foreach (Param p in f.Params)
                    {
                        if (p.Direction != Param.Dir.Out)
                        {
                            file.WriteLine("  mPacketComm->Write(" + p.Name + ");");
                        }
                    }
                    file.WriteLine("  mPacketComm->PacketDone();");
                    if (f.HasReturn)
                    {
                        file.WriteLine("  mPacketComm->EventReceiveWait();");
                        if (!f.ReturnVoid) file.WriteLine("  " + GetTypeInstanceMCPP(f.Return, "ret"));
                        file.WriteLine("  if(!mReceiving)");
                        file.WriteLine("  {");
                        file.WriteLine("    if(");
                        if (!f.ReturnVoid) file.WriteLine("      !mPacketComm->Read(ret) ||");
                        foreach (Param p in f.Params)
                        {
                            if (p.Direction != Param.Dir.In)
                            {
                                file.WriteLine("      !mPacketComm->Read(" + p.Name + ") ||");
                            }
                        }
                        file.WriteLine("      !mPacketComm->ReadDone())");
                        file.WriteLine("    {");
                        if (!f.ReturnVoid) file.WriteLine("      ret = " + GetInstanceMCPP(f.Return) + ";");
                        foreach (Param p in f.Params)
                        {
                            if (p.Direction != Param.Dir.In)
                            {
                                file.WriteLine("      " + p.Name + " = " + GetInstanceMCPP(p.Type) + ";");
                            }
                        }
                        file.WriteLine("      mPacketComm->HandleReceiveError();");
                        file.WriteLine("    }");
                        file.WriteLine("  }");
                        if (!f.ReturnVoid) file.WriteLine("  return ret;");
                    }
                    file.WriteLine("}");
                }
                file.WriteLine("}");
            }
        }

        private void GenerateMCppSkeletonHFile(Interface i)
        {
            string name = i.Name + "Skeleton";
            using (StreamWriter file = new OutputFile("MCPP/" + name + ".hpp"))
            {
                file.WriteLine(@"#pragma once");
                file.WriteLine();
                file.WriteLine("#include \"PacketCommunication.hpp\"");
                file.WriteLine("#include \"I" + i.Name + ".hpp\"");
                if (i.HasEvents)
                {
                    file.WriteLine("#include \"" + i.Name + "EventsProxy.hpp\"");
                }
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.WriteLine("public ref class " + name + " : public PacketCommunication::Callback");
                file.WriteLine("{");
                file.WriteLine("public:");
                file.WriteLine("  " + name + "(PacketCommunication^ packetComm);");
                file.WriteLine("  ~" + name + "();");
                if (i.HasEvents)
                {
                    file.WriteLine("  I" + i.Name + "Events^ GetEventsInterface();");
                }
                file.WriteLine("  void SetImpl(I" + i.Name + "^ impl);");
                file.WriteLine("  virtual void Receive(PacketCommunication^ pc) override;");
                file.WriteLine("private:");
                file.WriteLine("  I" + i.Name + "^ mImpl;");
                file.WriteLine("  PacketCommunication^ mPacketComm;");
                if (i.HasEvents)
                {
                    file.WriteLine("  " + i.Name + "EventsProxy^ mEventsProxy;");
                }
                file.WriteLine("};");
                file.WriteLine("}");
            }
        }

        private void GenerateMCppSkeletonCFile(Interface i)
        {
            string name = i.Name + "Skeleton";
            using (StreamWriter file = new OutputFile("MCPP/" + name + ".cpp"))
            {
                file.WriteLine("#include \"" + name + ".hpp\"");
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.WriteLine(name + "::" + name + "(PacketCommunication^ packetComm)");
                file.WriteLine(": Callback(" + (i.Id + 128) + ") // Interface Id");
                file.WriteLine(", mPacketComm(packetComm)");
                if (i.HasEvents)
                {
                    file.WriteLine(", mEventsProxy(gcnew " + i.Name + "EventsProxy(packetComm))");
                }
                file.WriteLine("{");
                file.WriteLine("  mPacketComm->Register(this);");
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine(name + "::~" + name + "()");
                file.WriteLine("{");
                if (i.HasEvents)
                {
                    file.WriteLine("  if (mImpl)");
                    file.WriteLine("  {");
                    foreach (Function f in i.Events)
                    {
                        file.WriteLine("    mImpl->" + f.Name + "Event -= gcnew I" +i.Name + "::" + f.Name + "Delegate(mEventsProxy, &" + i.Name + "EventsProxy::" + f.Name + ");");
                    }
                    file.WriteLine("  }");
                }
                file.WriteLine("  mPacketComm->Unregister(this);");
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("void " + name + "::SetImpl(I" + i.Name + "^ impl)");
                file.WriteLine("{");
                file.WriteLine("  mImpl = impl;");
                foreach (Function f in i.Events)
                {
                    file.WriteLine("  mImpl->" + f.Name + "Event += gcnew I" + i.Name + "::" + f.Name + "Delegate(mEventsProxy, &" + i.Name + "EventsProxy::" + f.Name + ");");
                }
                file.WriteLine("}");
                file.WriteLine();
                if (i.HasEvents)
                {
                    file.WriteLine("I" + i.Name + "Events^ " + name + "::GetEventsInterface()");
                    file.WriteLine("{");
                    file.WriteLine("  return mEventsProxy;");
                    file.WriteLine("}");
                    file.WriteLine();
                }
                file.WriteLine("void " + name + "::Receive(PacketCommunication^ pc)");
                file.WriteLine("{");
                List<Function> fs = i.Functions;
                if (fs.Any())
                {
                    file.WriteLine("  uint8_t func;");
                    file.WriteLine("  if(!pc->Read(func)) return;");
                    file.WriteLine("  switch(func)");
                    file.WriteLine("  {");
                    foreach (Function f in fs)
                    {
                        file.WriteLine("    case " + f.Id + ":");
                        file.WriteLine("    {");
                        List<Param> ps = f.Params;
                        foreach (Param p in ps)
                        {
                            file.WriteLine("      " + GetTypeInstanceMCPP(p.Type, p.Name));
                        }
                        if (f.HasReturn)
                        {
                            file.WriteLine("      uint8_t messageCnt;");
                            file.WriteLine("      if(!pc->Read(messageCnt)) return;");
                        }
                        foreach (Param p in ps)
                        {
                            if (p.Direction != Param.Dir.Out)
                            {
                                file.WriteLine("      if(!pc->Read(" + p.Name + ")) return;");
                            }
                        }
                        file.WriteLine("      if(!pc->ReadDone()) return;");
                        if (!f.ReturnVoid)
                        {
                            file.WriteLine("      " + GetTypeInstanceMCPP(f.Return, "ret"));
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
                            file.WriteLine("      pc->PacketStart(" + i.Id + ", " + f.Id + ");");
                            file.WriteLine("      pc->Write(messageCnt);");
                            if (!f.ReturnVoid) file.WriteLine("      pc->Write(ret);");
                            foreach (Param p in ps)
                            {
                                if (p.Direction != Param.Dir.In)
                                {
                                    file.WriteLine("      pc->Write(" + p.Name + ");");
                                }
                            }
                            file.WriteLine("      pc->PacketDone();");
                        }
                        file.WriteLine("      break;");
                        file.WriteLine("    }");
                    }
                    file.WriteLine("    default:");
                    file.WriteLine("      break;");
                    file.WriteLine("  }");
                }
                file.WriteLine("}");
                file.WriteLine("}");
            }
        }

        private void GenerateMCppDataTypeHFile(DataType d)
        {
            using (StreamWriter file = new OutputFile("MCPP/" + d.Name + ".hpp"))
            {
                string tag = d.Name.ToUpper() + "_H";
                file.WriteLine(@"#ifndef " + tag);
                file.WriteLine(@"#define " + tag);
                file.WriteLine();
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
                file.WriteLine("public ref class " + d.Name + ": public Serialize");
                file.WriteLine("{");
                file.WriteLine("public:");
                file.WriteLine("  " + d.Name + "();");
                file.WriteLine();
                file.WriteLine("  virtual void Write(PacketCommunication^ packetComm);");
                file.WriteLine("  virtual bool Read(PacketCommunication^ packetComm);");
                file.WriteLine();

                foreach (DataField p in d.Fields)
                {
                    if (p.Size > 1)
                    {
                        file.WriteLine("  array<" + GetTypeMCPP(p.Type) + ">^ " + p.Name + "; //[" + p.Size + "]");
                    }
                    else
                    {
                        file.WriteLine("  " + GetTypeMCPP(p.Type) + " " + p.Name + ";");
                    }
                }
                file.WriteLine("};");
                file.WriteLine("}");
                file.WriteLine("#endif");
            }
        }

        private void GenerateMCppDataTypeCFile(DataType d)
        {
            using (StreamWriter file = new OutputFile("MCPP/" + d.Name + ".cpp"))
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
                        file.WriteLine(p.Name + "(" + GetInstanceMCPP(p.Type) + ")");
                    }
                }
                file.WriteLine("{");
                foreach (DataField p in d.Fields)
                {
                    if (p.Size > 1)
                    {
                        file.WriteLine("  {");
                        file.WriteLine("    " + p.Name + " = gcnew array<" + GetTypeMCPP(p.Type) + ">(" + p.Size + ");");
                        file.WriteLine("    for(uint32_t i=0;i<" + p.Size + ";++i) " + p.Name + "[i] = " + GetInstanceMCPP(p.Type) + ";");
                        file.WriteLine("  }");
                    }
                }
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("void " + d.Name + "::Write(PacketCommunication^ packetComm)");
                file.WriteLine("{");
                foreach (DataField p in d.Fields)
                {
                    if (p.Size > 1)
                    {
                        file.WriteLine("  {");
                        file.WriteLine("    for(uint32_t i=0;i<" + p.Size + ";++i) packetComm->Write(" + p.Name + "[i]);");
                        file.WriteLine("  }");
                    }
                    else
                    {
                        file.WriteLine("  packetComm->Write(" + p.Name + ");");
                    }
                }
                file.WriteLine("}");
                file.WriteLine();
                file.WriteLine("bool " + d.Name + "::Read(PacketCommunication^ packetComm)");
                file.WriteLine("{");
                foreach (DataField p in d.Fields)
                {
                    if (p.Size > 1)
                    {
                        file.WriteLine("  {");
                        file.WriteLine("    for(uint32_t i=0;i<" + p.Size + ";++i) if(!packetComm->Read(" + p.Name + "[i])) return false;");
                        file.WriteLine("  }");
                    }
                    else
                    {
                        file.WriteLine("  if(!packetComm->Read(" + p.Name + ")) return false;");
                    }
                }
                file.WriteLine("  return true;");
                file.WriteLine("}");
                file.WriteLine("}");
            }
        }

        private void GenerateMCppEnumHFile(Enum en)
        {
            using (StreamWriter file = new OutputFile("MCPP/" + en.Name + ".hpp"))
            {
                string tag = en.Name.ToUpper() + "_H";
                file.WriteLine(@"#ifndef " + tag);
                file.WriteLine(@"#define " + tag);
                file.WriteLine();
                file.WriteLine("#include \"PacketCommunication.hpp\"");
                file.WriteLine();
                file.WriteLine("namespace erpc");
                file.WriteLine("{");
                file.WriteLine("public ref class " + en.Name + " : public Serialize");
                file.WriteLine("{");
                file.WriteLine("public:");
                file.WriteLine("  enum class Value");
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
                file.WriteLine();
                file.WriteLine("  Value value;");
                file.WriteLine("  virtual void Write(PacketCommunication^ pc)");
                file.WriteLine("  {");
                file.WriteLine("    pc->Write((int32_t)value);");
                file.WriteLine("  }");
                file.WriteLine("  virtual bool Read(PacketCommunication^ pc)");
                file.WriteLine("  {");
                file.WriteLine("      int32_t v;");
                file.WriteLine("      if (!pc->Read(v)) return false;");
                file.WriteLine("      value = (Value)v;");
                file.WriteLine("      return true;");
                file.WriteLine("  }");
                file.WriteLine("};");
                file.WriteLine("}");
                file.WriteLine("#endif");
            }
        }
    }
}

