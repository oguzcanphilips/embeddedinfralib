using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using System.Runtime.Serialization;

namespace FSM
{
    [Serializable]
    public class State : Selectable
    {
        public static readonly State Null = new State(); 
        static private readonly Pen mBorderPenInitial = new Pen(Color.Blue, 2);
        static private readonly Pen mBorderPen = new Pen(Color.Black, 1);
        static private readonly Pen mBorderPenSel = new Pen(Color.Red, 3);
        static private readonly Brush mTextBrush = new SolidBrush(Color.Black);
        private static readonly Brush[] mBackGround = new Brush[]
            {
                new SolidBrush(Color.FromArgb(250,250,210)),
                new SolidBrush(Color.FromArgb(250,210,250)),
                new SolidBrush(Color.FromArgb(210,250,250)),
                new SolidBrush(Color.FromArgb(210,210,250)),
                new SolidBrush(Color.FromArgb(250,210,210))
            };
        static private readonly Font mFont = new Font(FontFamily.GenericSansSerif, 10, FontStyle.Regular);

        private bool mIsInitial = false;
        private ActionsList mEnter = new ActionsList();
        private ActionsList mLeave = new ActionsList();
        private Rectangle mRec = new Rectangle(20, 20, 100, 50);
        [NonSerializedAttribute]
        private State mParent = null;
        public string mNameParent = null;

        public delegate void RemoveDelegate(State sender);
        [field: NonSerializedAttribute]
        public event RemoveDelegate RemoveEvent;

        public Point FindConnector(Point from)
        {
            return FindConnector(from, Point.Empty);
        }
        override public void Remove()
        {
            if (RemoveEvent != null) RemoveEvent(this);
        }
        public void Removed(State s)
        {
            if (s == mParent) Parent = null;
        }
        
        [OnSerializing]
        internal void OnSerializingMethod(StreamingContext context)
        {
            mNameParent = (mParent == null) ? ""  : mParent.Name;
        }
        [OnDeserialized]
        private void OnDeserializedMethod(StreamingContext context)
        {
            Parent = StateCollection.GetState(mNameParent);
            
            StateCollection.Add(this);
        }

        private void UpdateSize(Graphics g)
        {
            SizeF txtSize = g.MeasureString(GetText(), mFont);
            mRec.Width = (int)txtSize.Width + 11;
            mRec.Height = (int)txtSize.Height + 11;

            foreach (var child in Children())
            {
                child.UpdateSize(g);
                if ((child.mRec.X - 10) < mRec.X) mRec.X = child.mRec.X - 10;
                if ((child.mRec.Y - 10) < mRec.Y) mRec.Y = child.mRec.Y - 10;

                int w = child.mRec.X + child.mRec.Width - 1 + 10 - mRec.X;
                if (w > mRec.Width) mRec.Width = w;
                int h = child.mRec.Y + child.mRec.Height - 1 + 10 - mRec.Y;
                if (h > mRec.Height) mRec.Height = h;
            }
        }

        virtual public void Draw(Graphics g)
        {
            UpdateSize(g);

            Pen borderPen = Selected ? mBorderPenSel : Initial ? mBorderPenInitial : mBorderPen;
            int backIndex = 0;
            for (State p = Parent; p != null; p = p.Parent) backIndex = (backIndex + 1) % mBackGround.Length;

            g.FillRectangle(mBackGround[backIndex], mRec.X, mRec.Y + 5, mRec.Width, mRec.Height - 10);
            g.FillRectangle(mBackGround[backIndex], mRec.X + 5, mRec.Y, mRec.Width - 10, mRec.Height);
            g.FillPie(mBackGround[backIndex], mRec.Left, mRec.Top, 10, 10, 180, 90);
            g.FillPie(mBackGround[backIndex], mRec.Right - 10, mRec.Top, 10, 10, 270, 90);
            g.FillPie(mBackGround[backIndex], mRec.Left, mRec.Bottom - 10, 10, 10, 90, 90);
            g.FillPie(mBackGround[backIndex], mRec.Right - 10, mRec.Bottom - 10, 10, 10, 0, 90);

            g.DrawString(GetText(), mFont, mTextBrush, mRec.Left + 5, mRec.Top + 5);
            g.DrawLine(borderPen, mRec.Left + 5, mRec.Top, mRec.Right - 5, mRec.Top);
            g.DrawLine(borderPen, mRec.Left + 5, mRec.Bottom, mRec.Right - 5, mRec.Bottom);
            g.DrawLine(borderPen, mRec.Left, mRec.Top + 5, mRec.Left, mRec.Bottom - 5);
            g.DrawLine(borderPen, mRec.Right, mRec.Top + 5, mRec.Right, mRec.Bottom - 5);
            g.DrawArc(borderPen, mRec.Left, mRec.Top, 10, 10, 180, 90);
            g.DrawArc(borderPen, mRec.Right - 10, mRec.Top, 10, 10, 270, 90);
            g.DrawArc(borderPen, mRec.Left, mRec.Bottom - 10, 10, 10, 90, 90);
            g.DrawArc(borderPen, mRec.Right - 10, mRec.Bottom - 10, 10, 10, 0, 90);
        }
        virtual public Point FindConnector(Point from, Point exclude)
        {
            Point[] points = new Point[]
            {
                new Point(Center.X, mRec.Top),
                new Point(mRec.Left, mRec.Top),
                new Point(mRec.Left, Center.Y),
                new Point(mRec.Left, mRec.Bottom),
                new Point(mRec.Right, mRec.Top),
                new Point(mRec.Right, Center.Y),
                new Point(mRec.Right, mRec.Bottom),
                new Point(Center.X, mRec.Bottom)
            };

            double distanceMin = double.MaxValue;
            Point res = Point.Empty;
            foreach (var point in points)
            {
                if (!point.Equals(exclude))
                {
                    double distance = Util.CalcDistance(from, point);
                    if (distance < distanceMin)
                    {
                        distanceMin = distance;
                        res = point;
                    }
                }
            }
            return res;
        }
        override public bool IsSelected(Point pos)
        {
            return mRec.Contains(pos);
        }
        public override void Move(int dx, int dy)
        {
            mRec.X += dx;
            mRec.Y += dy;
        }
        override public Point Center
        {
            get
            {
                return new Point(mRec.X + mRec.Width / 2, mRec.Y + mRec.Height / 2);
            }
            set
            {
                mRec.X = value.X - mRec.Width / 2;
                mRec.Y = value.Y - mRec.Height / 2;
            }
        }
        public List<Event> GetInternalEvents()
        {
            return EventCollection.GetInternalEvents(this);
        }
        private string GetText()
        {
            string el = "";
            if (!mEnter.IsEmpty)
            {
                el += "<Enter>" + Environment.NewLine;
                el += mEnter + Environment.NewLine;
            }
            foreach (Event e in GetInternalEvents())
            {
                el += "<"+e.EventDefinition() + ">" + Environment.NewLine;
                el += e.Actions + Environment.NewLine;
            }
            if (!mLeave.IsEmpty)
            {
                el += "<Leave>" + Environment.NewLine;
                el += mLeave + Environment.NewLine;
            }
            return Name + Environment.NewLine + el;
        }

        public override bool Modify()
        {
            FormState fs = new FormState(this);
            StateCollection.Sort();
            bool res = DialogResult.OK == fs.ShowDialog();
            StateCollection.Sort();
            return res;
        }

        public Size Size { get { return mRec.Size; } }
        public ActionsList Enter { get { return mEnter; } set { mEnter = value; } }
        public ActionsList Leave { get { return mLeave; } set { mLeave = value; } }
        public bool Initial
        {
            get { return mIsInitial; }
            set
            {
                if (value == false) mIsInitial = false;
                else
                {
                    foreach (State node in StateCollection.GetStates())
                        (node).Initial = false;
                    mIsInitial = true;
                }
            }
        }
        public bool HasParent(State s)
        {
            State it = this;
            while (it != null)
            {
                if (it.Parent == s) return true;
                it = it.Parent;
            }
            return false;
        }
        public State Parent
        {
            get { return mParent; }
            set
            {
                if (mParent != null) mParent.RemoveEvent -= Removed;
                mParent = value;
                if (mParent != null) mParent.RemoveEvent += Removed;
            }
        }
        public bool HasChilderen()
        {
            return StateCollection.GetStates().Any(s => s.Parent == this);
        }
        public IEnumerable<State> Children()
        {

            List<State> children = new List<State>();
            foreach (State s in StateCollection.GetStates())
            {
                if(s.Parent == this) children.Add(s);
            }
            return children;
        }
        public IEnumerable<State> Leafs()
        {
            List<State> leafs = new List<State>();

            StateCollection.GetLeafs(this, ref leafs);
            return leafs;
        }
    }
}
