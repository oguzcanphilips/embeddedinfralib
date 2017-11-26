using System.Collections.Generic;
using System.Linq;

namespace FSM
{
    class EventCollection
    {
        static private readonly List<Event> mEvents = new List<Event>();
        static public IEnumerable<string> GetActions()
        {
            HashSet<string> actions = new HashSet<string>();
            foreach (Event ev in mEvents)
            {
                foreach (var action in ev.Actions.Actions)
                {
                    actions.Add(action);
                }
            }
            return actions;
        }
        static public IEnumerable<string> GetConditions()
        {
            HashSet<string> conds = new HashSet<string>();
            foreach (Event ev in mEvents)
            {
                foreach (string cond in ev.Guard.Conditions)
                {
                    conds.Add(cond);
                }
            }
            return conds;
        }
        static public IEnumerable<string> GetEventsAsStrings()
        {
            HashSet<string> events = new HashSet<string>();
            foreach (Event ev in mEvents)
            {
                if(!ev.Name.Equals("")) events.Add(ev.Name);
            }
            return events;
        }
        static public List<Event> GetEvents()
        {
            return mEvents;
        }
        static public List<Event> GetEventsFrom(State s)
        {
            return mEvents.Where(e => e.From == s).ToList();
        }
        static public List<Event> GetInternalEvents(State s)
        {
            return mEvents.Where(e => e.From == s && e.To == null).ToList();
        }

        static private void RemoveFromList(Event ev)
        {
            mEvents.Remove(ev);
        }
        static public void Remove(Event ev)
        {
            ev.Remove();
        }
        static public void Add(Event ev)
        {
            ev.RemoveEvent += RemoveFromList;
            mEvents.Add(ev);
        }
        static public void Clear()
        {
            List<Event> toBeRemoved = new List<Event>(mEvents);
            foreach (var ev in toBeRemoved)
            {
                Remove(ev);
            }
            mEvents.Clear();
        }
        
    }
}
