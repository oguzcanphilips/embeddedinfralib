using System;
using System.Linq;

namespace FSM
{
    class GeneratorCPP : Generator
    {
        private readonly Config mConfig;
        public GeneratorCPP(Config config) : base(config)
        {
            mConfig = config;
            using (var file = new OutputFile(config.Name + ".hpp"))
                GenerateHFile(file);

            using (var file = new OutputFile(config.Name + ".cpp"))
                GenerateCppFile(file);
        }

        private string StateName(string state)
        {
            return mConfig.Name + "_" + state;
        }
        
        protected override string StateName(State state)
        {
            return StateName(state.ToString());
        }

        private void GenerateHFile(OutputFile file)
        {
            string tag = mConfig.Name.ToUpper() + "_H";
            file.WriteLine("#ifndef " + tag);
            file.WriteLine("#define " + tag);
            file.WriteLine();
            file.WriteLine("namespace fsm");
            file.WriteLine("{");
            file.WriteLine("class " + mConfig.Name);
            file.WriteLine("{");
            file.WriteLine("protected:");
            file.WriteLine("  " + mConfig.Name + "();");
            file.WriteLine("  // should be called form the constructor of the derived class");
            file.WriteLine("  void SetInitialState();");
            file.WriteLine("public:");
            file.WriteLine("  virtual ~" + mConfig.Name + "(){}");
            file.WriteLine("  // events");
            foreach (var ev in EventCollection.GetEventsAsStrings())
            {
                file.WriteLine("  void " + ev + "();");
            }
            file.WriteLine("  enum State");
            file.WriteLine("  {");
            file.Write("    " + StateName("Unknown"));
            foreach (var state in StateCollection.GetStates().Where(state => !state.HasChilderen()))
            {
                file.WriteLine(",");
                file.Write("    " + StateName(state));
            }
            file.WriteLine();
            file.WriteLine("  };");
            file.WriteLine("  State GetCurrentState() const;");
            file.WriteLine("protected:");
            file.WriteLine("  // conditions");
            foreach (var c in GetConditions())
            {
                file.WriteLine("  virtual bool " + c + "() = 0;");
            }
            file.WriteLine("  // actions");
            foreach (var a in GetActions())
            {
                file.WriteLine("  virtual void " + a + "() = 0;");
            }
            file.WriteLine("private:");
            file.WriteLine("  void StartEventProcessing();");
            file.WriteLine("  void EndEventProcessing();");
            foreach (var s in StateCollection.GetStates())
            {
                if (NeedsEnterMethod(s)) file.WriteLine("  void Enter_" + s + "();");
                if (NeedsLeaveMethod(s)) file.WriteLine("  void Leave_" + s + "();");
            }
            file.WriteLine("  State currentState;");
            file.WriteLine("  // events implementation");
            if (!mConfig.UseAssert)
            {
                foreach (var ev in EventCollection.GetEventsAsStrings())
                {
                    file.WriteLine("  void " + ev + "Impl();");
                }
                file.WriteLine("  class EventFunc");
                file.WriteLine("  {");
                file.WriteLine("    friend class " + mConfig.Name + ";");
                file.WriteLine("    void(" + mConfig.Name + "::*func)(void);");
                file.WriteLine("    EventFunc* next;");
                file.WriteLine("    EventFunc(void(" + mConfig.Name + "::*f)(void)) : func(f){}");
                file.WriteLine("  };");
                foreach (var ev in EventCollection.GetEventsAsStrings())
                {
                    file.WriteLine("  EventFunc Ev" + ev + ";");
                }
                file.WriteLine("  EventFunc* mPendingEvents;");
                file.WriteLine("  void AddEvent(EventFunc* ev);");
                file.WriteLine("  void ProcessEvents();");
            }
            file.WriteLine("  bool mIsProcessingEvents;");
            file.WriteLine("};");
            file.WriteLine("}");
            file.WriteLine();
            file.WriteLine("#endif");
        }

        private void GenerateCppFile(OutputFile file)
        {
            string scope = mConfig.Name + "::";
            file.WriteLine("#include\""+mConfig.Name+".hpp\"");
            file.WriteLine("#include<cassert>");
            file.WriteLine();
            file.WriteLine("#ifndef DEBUG_FSM");
            file.WriteLine("  #define DebugFSM(TXT)");
            file.WriteLine("#else");
            file.WriteLine("  extern void DebugFSM(const char* txt);"); ;
            file.WriteLine("#endif");
            file.WriteLine();
            file.WriteLine("namespace fsm");
            file.WriteLine("{");
            file.WriteLine(scope + mConfig.Name + "()");
            file.WriteLine(": currentState("+StateName("Unknown")+")");
            if (!mConfig.UseAssert)
            {
                foreach (var ev in EventCollection.GetEventsAsStrings())
                {
                    file.WriteLine(", Ev" + ev + "(&" + scope + ev + "Impl)");
                }
                file.WriteLine(", mPendingEvents(0)");
            }
            file.WriteLine(", mIsProcessingEvents(true)");
            file.WriteLine("{");
            file.WriteLine("}");
            foreach (var s in StateCollection.GetStates())
            {
                if (NeedsEnterMethod(s))
                {
                    file.WriteLine("void " + scope + "Enter_" + s + "()");
                    file.WriteLine("{");
                    foreach (var action in s.Enter.Actions)
                    {
                        file.WriteLine("  " + action + "();");
                    }
                    file.WriteLine("}");
                }

                if (NeedsLeaveMethod(s))
                {
                    file.WriteLine("void " + scope + "Leave_" + s + "()");
                    file.WriteLine("{");
                    foreach (var action in s.Leave.Actions)
                    {
                        file.WriteLine("  " + action + "();");
                    }
                    file.WriteLine("}");
                }
            }
            if (mConfig.UseAssert)
            {
                file.WriteLine("void " + scope + "StartEventProcessing()");
                file.WriteLine("{");
                file.WriteLine("  assert(!mIsProcessingEvents);");
                file.WriteLine("  mIsProcessingEvents = true;");
                file.WriteLine("}");
                file.WriteLine("void " + scope + "EndEventProcessing()");
                file.WriteLine("{");
                file.WriteLine("  mIsProcessingEvents = true;");
                file.WriteLine("}");
            }
            else
            {
                file.WriteLine("void " + scope + "StartEventProcessing()");
                file.WriteLine("{");
                file.WriteLine("}");
                file.WriteLine("void " + scope + "EndEventProcessing()");
                file.WriteLine("{");
                file.WriteLine("}");
                file.WriteLine("void " + scope + "AddEvent(EventFunc* ev)");
                file.WriteLine("{");
                file.WriteLine("  if(mPendingEvents == 0) mPendingEvents = ev;");
                file.WriteLine("  else");
                file.WriteLine("  {");
                file.WriteLine("     EventFunc* pos = mPendingEvents;");
                file.WriteLine("     for(EventFunc* it = mPendingEvents; it ; it=it->next)");
                file.WriteLine("     {");
                file.WriteLine("       if(it == ev) return;");
                file.WriteLine("       pos = it;");
                file.WriteLine("     }");
                file.WriteLine("     pos->next = ev;");
                file.WriteLine("  }");
                file.WriteLine("  ev->next = 0;");
                file.WriteLine("  ProcessEvents();");
                file.WriteLine("}");
                file.WriteLine("void " + scope + "ProcessEvents()");
                file.WriteLine("{");
                file.WriteLine("  if(!mIsProcessingEvents)");
                file.WriteLine("  {");
                file.WriteLine("    mIsProcessingEvents = true;");
                file.WriteLine("    while(mPendingEvents)");
                file.WriteLine("    {");
                file.WriteLine("      EventFunc* ef = mPendingEvents;");
                file.WriteLine("      mPendingEvents = mPendingEvents->next;");
                file.WriteLine("      (this->*ef->func)();");
                file.WriteLine("    }");
                file.WriteLine("    mIsProcessingEvents = false;");
                file.WriteLine("  }");
                file.WriteLine("}");
                file.WriteLine();
            }
            file.WriteLine(scope+"State "+ scope+"GetCurrentState() const");
            file.WriteLine("{");
            file.WriteLine("  return currentState;");
            file.WriteLine("}");

            ImplementEvents(file, "", scope, "", "bool");
            
            file.WriteLine("void " + scope + "SetInitialState()");
            file.WriteLine("{");
            file.WriteLine("  mIsProcessingEvents = true;");
            State initialState = StateCollection.GetInitialState();
            State it = initialState.Parent;
            string enters = "";
            if (NeedsEnterMethod(initialState))
            {
                enters = "  Enter_" + initialState.Name + "();" + Environment.NewLine;
            }
            while (it != null)
            {
                if (NeedsEnterMethod(it))
                {
                    enters = "  Enter_" + it + "();" + Environment.NewLine + enters;
                }
                it = it.Parent;
            }
            file.Write(enters);
            file.WriteLine("  DebugFSM(\"" + StateName(initialState) + "\");");
            file.WriteLine("  currentState = " + StateName(initialState) + ";");
            file.WriteLine("  mIsProcessingEvents = false;");
            if (!mConfig.UseAssert)
            {
                file.WriteLine("  ProcessEvents();");
            }
            file.WriteLine("}");
            file.WriteLine("}");
        }
    }
}

