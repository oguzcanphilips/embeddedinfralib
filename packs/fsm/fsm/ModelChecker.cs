using System;
using System.Collections.Generic;
using System.Linq;

namespace FSM
{
    class ModelChecker
    {
        class Eval
        {
            public string Name;
            public bool Value;
            public Eval(string name, bool v)
            {
                Name = name;
                Value = v;
            }
        }

        private static string CheckModelName(Config config)
        {
            if (config.Name.Equals(""))
            {
                return "Model does not have a name" + Environment.NewLine;
            }
            return "";
        }

        private static string FindDuplicates(List<string> items, string errPrefix)
        {
            var duplicates = new List<string>();
            string err = "";
            foreach (var i in items)
            {
                if (items.Count(chk => chk.Equals(i)) > 1)
                {
                    bool alreadyFound = duplicates.Any(duplicate => duplicate.Equals(i));
                    if (!alreadyFound)
                    {
                        err += errPrefix + i + Environment.NewLine;
                        duplicates.Add(i);
                    }
                }
            }
            return err;
        }
        private static string CheckGuards(List<Event> events, string errPrefix)
        {
            HashSet<Guard> guards = new HashSet<Guard>();
            HashSet<string> conds = new HashSet<string>();
            foreach (var e in events)
            {
                guards.Add(e.Guard);
                foreach (string c in e.Guard.Conditions)
                {
                    conds.Add(c);                    
                }
            }
            List<Eval> evalList = new List<Eval>();
            foreach (string cond in conds)
            {
                evalList.Add(new Eval(cond, false));
            }
            bool run = true;
            while (run)
            {
                bool alreadyTriggered = false;
                foreach (var guard in guards)
                {
                    foreach (var eval in evalList)
                    {
                        guard.Set(eval.Name, eval.Value);
                    }
                    try
                    {
                        if (guard.Evaluate())
                        {
                            if (alreadyTriggered)
                            {
                                return errPrefix + " guards not unique for '" + events.First().Name + "'" + "(to: '"+events.First().To+"')"+
                                       Environment.NewLine;
                            }
                            alreadyTriggered = true;
                        }
                    }
                    catch (Exception e)
                    {
                        return e.Message;
                    }
                }
                bool carry = true; // increment
                bool allZero = true;
                foreach (var eval in evalList)
                {
                    switch ((carry ? 1 : 0) + (eval.Value ? 1 : 0))
                    {
                    case 0: eval.Value = false; carry = false; break;
                    case 1: eval.Value = true;  carry = false; break;
                    case 2: eval.Value = false; carry = true; break;
                    }
                    allZero &= !eval.Value;
                }
                if (allZero) run = false;
            }
            return "";
        }

        private static string FindDuplicates(List<Event> events, string errPrefix)
        {
            string err = "";
            while(events.Count>0)
            {
                Event e = events.First();
                var duplicates = new List<Event>();
                foreach (var el in events)
                {
                    if (el.Name.Equals(e.Name))
                    {
                        duplicates.Add(el);
                    }
                }
                if (duplicates.Count > 1)
                {
                    err += CheckGuards(duplicates, errPrefix);
                }

                foreach (var duplicate in duplicates)
                {
                    events.Remove(duplicate);
                }
            }
            return err;
        }

        private static string CheckDuplicateEventsFromStates()
        {
            string err = "";
            foreach (var c in StateCollection.GetStates())
            {
                List<Event> events = new List<Event>();
                foreach (Event ev in EventCollection.GetEvents())
                {
                    if (ev.From == c)
                    {
                        events.Add(ev);
                    }
                }
                err += FindDuplicates(events, "Duplicate event from '" + c.Name + "' : ");
            }
            return err;
        }

        private static string CheckDuplicateStates()
        {
            List<string> states = StateCollection.GetStates().Select(c => c.Name).ToList();
            return FindDuplicates(states, "Duplicate State : ");
        }

        static private string CheckInitialState()
        {
            if (null == StateCollection.GetInitialState())
            {
                return "Missing initial state" + Environment.NewLine;
            }
            return "";
        }

        private static string CheckEventsToLeafs()
        {
            string err = "";
            foreach (var e in EventCollection.GetEvents())
            {
                if (e.To != null && e.To.HasChilderen())
                {
                    err += "Invalid event '" + e.Name + "' from: '" + e.From.Name + "' to: '" + e.To.Name + "'" + Environment.NewLine;
                }
            }
            return err;
        }
        private static string CheckEventsFromTransitionalStateOrNormalState()
        {
            string err = "";
            foreach (var e in EventCollection.GetEvents())
            {
                if (e.From.GetType() == typeof(TransitionalState))
                {
                    if (!e.Name.Equals(""))
                        err += "Invalid event '" + e.Name + "' from: '" + e.From.Name + "' to: '" + e.To.Name + "' (Name should be empty)" + Environment.NewLine;
                }
                else
                {
                    if (e.Name.Equals(""))
                        err += "Unnamed event from: '" + e.From.Name + "' to: '" + e.To.Name + "'" + Environment.NewLine;
                }
            }
            return err;            
        }
        static public string CheckModel(Config config)
        {
            string err = CheckModelName(config);
            err += CheckInitialState();
            err += CheckDuplicateEventsFromStates();
            err += CheckDuplicateStates();
            err += CheckEventsToLeafs();
            err += CheckEventsFromTransitionalStateOrNormalState();
            return err;
        }
    }
}
