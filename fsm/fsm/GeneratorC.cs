using System;
using System.Linq;

namespace FSM
{
    class GeneratorC : Generator
    {
        private Config mConfig;

        public GeneratorC(Config config) : base(config)
        {
            mConfig = config;
            using (var file = new OutputFile(config.Name + ".h"))
                GenerateHFile(file);

            using (var file = new OutputFile(config.Name + ".c"))
                GenerateCFile(file);
        }

        private string StateName(string state)
        {
            return mConfig.Name + "_State_" + state;
        }
        protected override string StateName(State state)
        {
            return StateName(state.ToString());
        }

        private void GenerateHFile(OutputFile file)
        {
            string tag = mConfig.Name.ToUpper() + "_H";
            string scope = mConfig.Name + "_";
            file.WriteLine("#ifndef " + tag);
            file.WriteLine("#define " + tag);
            file.WriteLine();
            file.WriteLine("typedef enum");
            file.WriteLine("{");
            file.Write("  " + StateName("Unknown"));
            foreach (var state in StateCollection.GetStates().Where(state => !state.HasChilderen()))
            {
                file.WriteLine(",");
                file.Write("  " + StateName(state));
            }
            file.WriteLine();
            file.WriteLine("} " + scope + "State;");
            file.WriteLine();

            file.WriteLine("void " + scope + "SetInitialState(void);");
            file.WriteLine(scope + "State " + scope + "GetCurrentState(void);");
            file.WriteLine("/* events */");
            foreach (var ev in EventCollection.GetEventsAsStrings())
            {
                file.WriteLine("void " + scope+ev + "(void);");
            }
            file.WriteLine("/* conditions */");
            foreach (var c in GetConditions())
            {
                file.WriteLine("/*virtual*/ int " + scope + c + "(void);");
            }
            file.WriteLine("/* actions */");
            foreach (var a in GetActions())
            {
                file.WriteLine("/*virtual*/ void " + scope + a + "(void);");
            }
            file.WriteLine("#endif");
        }

        private void GenerateCFile(OutputFile file)
        {
            string scope = mConfig.Name + "_";
            string eventType = scope + "Event";
            file.WriteLine("#include\""+mConfig.Name+".h\"");
            file.WriteLine("#include<assert.h>");
            file.WriteLine();

            file.WriteLine("#ifndef DEBUG_FSM");
            file.WriteLine("  #define DebugFSM(TXT)");
            file.WriteLine("#else");
            file.WriteLine("  extern void DebugFSM(const char* txt);"); ;
            file.WriteLine("#endif");
            file.WriteLine();
            file.WriteLine("static " + scope + "State currentState = "+StateName("Unknown")+";");
            string varIsProcessingEvents = "Ev" + scope + "IsProcessingEvents";
            file.WriteLine("static int " + varIsProcessingEvents + " = 1;");
            if (!mConfig.UseAssert)
            {
                file.WriteLine("typedef struct _" + eventType);
                file.WriteLine("{");
                file.WriteLine("  struct _" + eventType + "* next;");
                file.WriteLine("  void(*func)(void);");
                file.WriteLine("} " + eventType + ";");
                foreach (var evs in EventCollection.GetEventsAsStrings())
                {
                    file.WriteLine("static " + eventType + " Ev" + scope + evs + ";");
                }
                string varPendingEvents = "Ev" + scope + "PendingEvents";            
                file.WriteLine("static " + eventType + "* " + varPendingEvents + " = 0;");                
                file.WriteLine("static void " + scope + "ProcessEvents(void);");
                file.WriteLine("static void " + scope + "AddEvent(" + eventType + "* ev);");
                file.WriteLine("  // events implementation");
                foreach (var ev in EventCollection.GetEventsAsStrings())
                {
                    file.WriteLine("static void " + scope + ev + "Impl(void);");
                }
                file.WriteLine("static void " + scope + "AddEvent(" + eventType + "* ev)");
                file.WriteLine("{");
                file.WriteLine("  if(" + varPendingEvents + " == 0) " + varPendingEvents + " = ev;");
                file.WriteLine("  else");
                file.WriteLine("  {");
                file.WriteLine("    " + eventType + "* pos = " + varPendingEvents + ";");
                file.WriteLine("    " + eventType + "* it = " + varPendingEvents + ";");
                file.WriteLine("    for(; it ; it=it->next)");
                file.WriteLine("    {");
                file.WriteLine("      if(it == ev) return;");
                file.WriteLine("      pos = it;");
                file.WriteLine("    }");
                file.WriteLine("    pos->next = ev;");
                file.WriteLine("  }");
                file.WriteLine("  ev->next = 0;");
                file.WriteLine("  " + scope + "ProcessEvents();");
                file.WriteLine("}");
                file.WriteLine("static void " + scope + "ProcessEvents()");
                file.WriteLine("{");
                file.WriteLine("  if(!" + varIsProcessingEvents + ")");
                file.WriteLine("  {");
                file.WriteLine("    " + varIsProcessingEvents + " = 1;");
                file.WriteLine("    while(" + varPendingEvents + ")");
                file.WriteLine("    {");
                file.WriteLine("     " + eventType + "* ef = " + varPendingEvents + ";");
                file.WriteLine("      " + varPendingEvents + " = " + varPendingEvents + "->next;");
                file.WriteLine("      (*ef->func)();");
                file.WriteLine("    }");
                file.WriteLine("    " + varIsProcessingEvents + " = 0;");
                file.WriteLine("  }");
                file.WriteLine("}");
                file.WriteLine("static void StartEventProcessing()");
                file.WriteLine("{");
                file.WriteLine("}");
                file.WriteLine("static void EndEventProcessing()");
                file.WriteLine("{");
                file.WriteLine("}");
            }
            else
            {
                file.WriteLine("static void StartEventProcessing()");
                file.WriteLine("{");
                file.WriteLine("  assert(" + varIsProcessingEvents + " == 0);");
                file.WriteLine("  " + varIsProcessingEvents + " = 1;");
                file.WriteLine("}");
                file.WriteLine("static void EndEventProcessing()");
                file.WriteLine("{");
                file.WriteLine("  " + varIsProcessingEvents + " = 0;");
                file.WriteLine("}");
            }
            file.WriteLine();
            foreach (var s in StateCollection.GetStates())
            {
                if (NeedsEnterMethod(s))
                {
                    file.WriteLine("static void " + scope + "Enter_" + s + "(void)");
                    file.WriteLine("{");
                    foreach (var action in s.Enter.Actions)
                    {
                        file.WriteLine("  " + scope + action + "();");
                    }
                    file.WriteLine("}");
                }
                if (NeedsLeaveMethod(s))
                {
                    file.WriteLine("static void " + scope + "Leave_" + s + "(void)");
                    file.WriteLine("{");
                    foreach (var action in s.Leave.Actions)
                    {
                        file.WriteLine("  " + scope + action + "();");
                    }
                    file.WriteLine("}");
                }
            }

            file.WriteLine(scope + "State " + scope + "GetCurrentState(void)");
            file.WriteLine("{");
            file.WriteLine("  return currentState;");
            file.WriteLine("}");

            ImplementEvents(file, scope, scope, "void", "int");
            
            file.WriteLine("void " + scope + "SetInitialState(void)");
            file.WriteLine("{");
            file.WriteLine("  " + varIsProcessingEvents + " = 1;");
            if (!mConfig.UseAssert)
            {
                foreach (var evs in EventCollection.GetEventsAsStrings())
                {
                    file.WriteLine("  Ev" + scope + evs + ".func = &" + scope + evs + "Impl;");
                }
            }
            State initialState = StateCollection.GetInitialState();
            State it = initialState.Parent;
            string enters = "";
            if (NeedsEnterMethod(initialState))
            {
                enters += "  " + scope + "Enter_" + initialState.Name + "();" + Environment.NewLine;
            }
            while (it != null)
            {
                if (NeedsEnterMethod(it))
                {
                    enters = "  " + scope + "Enter_" + it + "();" + Environment.NewLine + enters;
                }
                it = it.Parent;
            }
            file.Write(enters);
            file.WriteLine("  DebugFSM(\"" + StateName(initialState) + "\");");
            file.WriteLine("  currentState = " + StateName(initialState) + ";");
            file.WriteLine("  " + varIsProcessingEvents + " = 0;");
            if (!mConfig.UseAssert)
                file.WriteLine("  " + scope + "ProcessEvents();");
            file.WriteLine("}");
        }
    }
}

