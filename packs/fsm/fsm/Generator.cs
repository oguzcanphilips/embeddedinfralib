using System;
using System.Collections.Generic;
using System.Linq;

namespace FSM
{
    internal abstract class Generator
    {
        private readonly Config mConfig;
        protected Generator(Config config)
        {
            mConfig = config;
        }

        public class GenEvent
        {
            private readonly Event _source;
            private State _from = null;
            private State _to = null;
            private Guard _guard;
            private ActionsList _actions;

            public State RealFrom
            {
                get { return _source.From; }
            }

            public State From
            {
                get { return _from ?? _source.From; }
                set { _from = value; }
            }

            public State To
            {
                get { return _to ?? _source.To; }
                set { _to = value; }
            }

            public ActionsList Actions
            {
                get { return _actions; }
            }

            public string Name
            {
                get { return _source.Name; }
            }

            public Guard Guard
            {
                get { return _guard; }
            }
            public string GuardExpression
            {
                get { return _guard.Expression; }
                set { _guard.Expression = value; }
            }
            public void AddActions(ActionsList al)
            {
                _actions.AddActions(al);
            }
            public GenEvent(Event source)
            {
                _guard = source.Guard.Clone();
                _source = source;
                _actions = source.Actions.Clone();
            }
            public GenEvent(GenEvent ge)
            {
                _from = ge._from;
                _to = ge._to;
                _source = ge._source;
                _actions = ge._actions.Clone();
                _guard = ge._guard.Clone();
            }
        }

        protected IEnumerable<string> GetConditions()
        {
            HashSet<string> conds = new HashSet<string>();
            foreach (string cond in EventCollection.GetConditions())
            {
                conds.Add(cond);
            }
            return conds;
        }

        protected IEnumerable<string> GetActions()
        {
            HashSet<string> actions = new HashSet<string>();
            foreach (var action in EventCollection.GetActions())
            {
                actions.Add(action);
            }
            foreach (var action in StateCollection.GetActions())
            {
                actions.Add(action);
            }
            return actions;
        }

        protected IEnumerable<GenEvent> GetGeneratorEvents(string name)
        {
            List<GenEvent> events = new List<GenEvent>();
            foreach (var state in StateCollection.GetStates())
            {
                if (state.HasChilderen()) continue;
                if (state.GetType() == typeof (TransitionalState)) continue;
                for (State it = state; it != null; it = it.Parent)
                {
                    List<GenEvent> extEvents = new List<GenEvent>();
                    foreach (var ev in EventCollection.GetEventsFrom(it))
                    {
                        if (!ev.Name.Equals(name)) continue;
                        GenEvent ge = new GenEvent(ev);
                        ge.From = state;
                        if (ev.To == null) events.Add(ge); // internal events first in the list.
                        else extEvents.Add(ge);
                    }
                    events.AddRange(extEvents);
                }
            }
            // remove TransitionalStates
            bool foundTransitionalState = false;
            do
            {
                foundTransitionalState = false;
                List<GenEvent> expandedEvents = new List<GenEvent>();
                foreach (GenEvent genEvent in events)
                {
                    if (genEvent.To!=null && genEvent.To.GetType() == typeof(TransitionalState))
                    {
                        var toEvents = EventCollection.GetEventsFrom(genEvent.To);
                        foreach (Event ev in toEvents)
                        {
                            GenEvent expGenEvent = new GenEvent(genEvent);
                            if (ev.Guard.Conditions.Any())
                            {
                                if (expGenEvent.Guard.Conditions.Any())
                                {
                                    expGenEvent.GuardExpression = "(" + expGenEvent.GuardExpression + ") & ";
                                }
                                expGenEvent.GuardExpression += "("+ev.Guard.Expression+")";
                            }
                            expGenEvent.To = ev.To;
                            expGenEvent.AddActions(ev.Actions);
                            expandedEvents.Add(expGenEvent);
                        }
                        foundTransitionalState = true;
                    }
                    else
                    {
                        expandedEvents.Add(genEvent);
                    }
                }
                events = expandedEvents;
            } while (foundTransitionalState);
            return events;
        }

        protected bool NeedsEnterMethod(State s)
        {
            return (s.Enter.Actions.Any());
        }

        protected bool NeedsLeaveMethod(State s)
        {
            return s.Leave.Actions.Any();
        }

        protected abstract string StateName(State state);

        private void AppendEvent(string indent, string scope, OutputFile file, GenEvent ev)
        {
            if (ev == null)
            {
                file.WriteLine(indent + "assert(false);/* missing event definition */");
                return;
            }
            if (ev.To == null)
            {
                foreach (var action in ev.Actions.Actions)
                {
                    file.WriteLine(indent + scope + action + "();");
                }
                return;
            }
            State it = ev.From;
            bool foundRealFrom = false;
            while (it != null)
            {
                if (ev.To.HasParent(it) && foundRealFrom) break;
                if (NeedsLeaveMethod(it))
                {
                    file.WriteLine(indent + "DebugFSM(\"" + mConfig.Name + "_Leave_" + it + "\");");
                    file.WriteLine(indent + scope + "Leave_" + it + "();");
                }
                foundRealFrom |= (it == ev.RealFrom);
                it = it.Parent;
            }
            foreach (var action in ev.Actions.Actions)
            {
                file.WriteLine(indent + scope + action + "();");
            }

            it = ev.To;
            string enters = "";
            while (it != null)
            {
                if (ev.RealFrom.HasParent(it)) break;
                if (NeedsEnterMethod(it))
                {
                    enters = indent + "DebugFSM(\"" + mConfig.Name + "_Enter_" + it + "\");" + Environment.NewLine +
					         indent + scope + "Enter_" + it + "();" + Environment.NewLine + 
                             enters;
                }
                it = it.Parent;
            }
            
            file.Write(enters);
            file.WriteLine(indent + "DebugFSM(\"" + StateName(ev.To) + "\");");
            file.WriteLine(indent + "currentState = " + StateName(ev.To) + ";");
        }
        private IEnumerable<string> GetConditionsFromStateForEvent(State state, string evs)
        {
            HashSet<string> result = new HashSet<string>();
            bool inDeadCode = false;
            foreach (var gev in GetGeneratorEvents(evs))
            {
                if (gev.From == state && !inDeadCode)
                {
                    inDeadCode = !gev.Guard.Conditions.Any();
                    foreach (var condition in gev.Guard.Conditions)
                    {
                        result.Add(condition);
                    }
                }
            }
            return result;
        }
        protected void ImplementEvents(OutputFile file, string prefix, string scope, string Void, string Bool)
        {
            foreach (var evs in EventCollection.GetEventsAsStrings())
            {
                file.WriteLine();
                file.WriteLine("void " + scope + evs + "(" + Void + ")");
                if (!mConfig.UseAssert)
                {
                    file.WriteLine("{");
                    file.WriteLine("  " + prefix + "AddEvent(&Ev" + prefix + evs + ");");
                    file.WriteLine("}");
                    file.WriteLine("void " + scope + evs + "Impl(" + Void + ")");
                }
                file.WriteLine("{");
                string prevFrom = "";
                bool inDeadCode = false;
                file.WriteLine("  StartEventProcessing();");
                file.WriteLine("  switch(currentState)");
                file.WriteLine("  {");
                foreach (var gev in GetGeneratorEvents(evs))
                {
                    string stateName = StateName(gev.From);
                    if (!prevFrom.Equals(stateName))
                    {
                        if (!prevFrom.Equals(""))
                        {
                            file.WriteLine("    break;");
                            file.WriteLine("  }");
                        }
                        inDeadCode = false;
                        prevFrom = stateName;
                        file.WriteLine("  case " + stateName + ":");
                        file.WriteLine("  {");
                        foreach (string cond in GetConditionsFromStateForEvent(gev.From, evs))
                        {
                            file.WriteLine("    " + Bool + " cond" + cond + " = " + prefix + cond + "();");
                        }
                    }
                    if (!inDeadCode)
                    {
                        if (!gev.Guard.Conditions.Any())
                        {
                            AppendEvent("    ", prefix, file, gev);
                            inDeadCode = true;
                        }
                        else
                        {
                            file.WriteLine("    if(" + gev.Guard.GetCode("cond") + ")");
                            file.WriteLine("    {");
                            AppendEvent("      ", prefix, file, gev);
                            file.WriteLine("      break;");
                            file.WriteLine("    }");
                        }
                    }
                }
                file.WriteLine("    break;");
                file.WriteLine("  }");
                file.WriteLine("  default: break;");
                file.WriteLine("  }");
                file.WriteLine("  EndEventProcessing();");
                file.WriteLine("}");
                file.WriteLine();                
            }
        }
    }
}

