using System;
using System.Drawing;

namespace FSM
{
    [Serializable]
    public abstract class Selectable
    {
        [field: NonSerializedAttribute]
        private bool mSelected = false;

        private string mName;
        public bool Selected { get { return mSelected; } set { mSelected = value; } }
        public abstract bool IsSelected(Point pos);
        public abstract void Move(int dx, int dy);

        public virtual void ResetPosition()
        {
        }

        public virtual void Down()
        {
        }
        public virtual void Up()
        {
        }

        public abstract Point Center
        {
            get;
            set;
        }
        public override string ToString()
        {
            return Name;
        }
        public string Name
        {
            get { return mName ?? ""; }
            set { mName = value; }
        }
        public abstract void Remove();
        public abstract bool Modify();
    }
}
