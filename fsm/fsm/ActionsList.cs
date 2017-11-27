using System;
using System.Collections.Generic;

namespace FSM
{
    [Serializable]
    public class ActionsList
    {
        private string mActions = "";
        private IEnumerable<string> ItemsForString(string input)
        {
            List<string> res = new List<string>();
            foreach (var item in input.Split())
            {
                var str = item.Trim();
                if (!str.Equals("")) res.Add(item.Trim());
            }
            return res;            
        }
        public ActionsList Clone()
        {
            ActionsList clone = new ActionsList();
            clone.mActions = (string)mActions.Clone();
            return clone;
        }

        public void AddActions(ActionsList al)
        {
            mActions += Environment.NewLine + al.mActions;
        }
        public bool IsEmpty
        {
            get { return mActions.Equals(""); }
        }
        public IEnumerable<string> Actions
        {
            get
            {
                return ItemsForString(mActions);
            }
        }
        public override string ToString()
        {
            return AsString;
        }
        public string AsString
        {
            get 
            { 
                return mActions;
            }
            set
            {
                mActions = "";
                foreach (var action in ItemsForString(value))
                {
                    if (!mActions.Equals("")) mActions += Environment.NewLine;
                    mActions += action;
                }
            }
        }
    }
}
