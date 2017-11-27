using System;

namespace FSM
{
    [Serializable]
    public class Config
    {
        private string mName = "";
        private bool mUseAssert = false;
        public string Name
        {
            get { return mName; }
            set { mName = value; }
        }
        public bool UseAssert
        {
            get { return mUseAssert; }
            set { mUseAssert = value; }
        }
    }
}
