using System;
using System.Collections.Generic;
using System.Linq;

namespace FSM
{
    class StateCollection
    {
        static private List<State> mStates = new List<State>();
        static public IEnumerable<string> GetActions()
        {
            var actions = new HashSet<string>();
            foreach (var s in mStates)
            {
                foreach (var action in (s.Enter + Environment.NewLine + s.Leave).Split())
                {
                    actions.Add(action);
                }
            }
            actions.Remove("");
            return actions;
        }

        static public List<State> GetStates()
        {
            return mStates;
        }

        static public State GetInitialState()
        {
            return mStates.FirstOrDefault(st => st.Initial);
        }

        static private void RemoveFromList(State node)
        {
            mStates.Remove(node);
        }

        static public void Remove(State node)
        {
            node.Remove();
        }
        
        static public void Add(State node)
        {
            node.RemoveEvent += RemoveFromList;
            mStates.Add(node);
            Sort();
        }
        
        static public void Clear()
        {
            var toBeRemoved = new List<State>(mStates);
            foreach (var state in toBeRemoved)
            {
                Remove(state);
            }
            mStates.Clear();
        }
        static private void AddState(ref List<State> nodes, State s)
        {
            if (s.Parent != null)
            {
                AddState(ref nodes, s.Parent);
            }
            if (!nodes.Contains(s))
            {
                nodes.Add(s);
            }
        }
        static public void Sort()
        {
            List<State> nodes = new List<State>();

            foreach (var s in mStates)
            {
                AddState(ref nodes, s);
            }
            mStates = nodes;
        }

        static public void GetLeafs(State parent, ref List<State> children)
        {
            foreach (State s in mStates)
            {
                if (s.Parent == parent)
                {
                    if (s.HasChilderen())
                    {
                        GetLeafs(s, ref children);
                    }
                    else
                    {
                        children.Add(s);
                    }
                }
            }
        }

        static public State GetState(string name)
        {
            return mStates.Where(state => state.Name.Equals(name)).FirstOrDefault();
        }
    }
}
