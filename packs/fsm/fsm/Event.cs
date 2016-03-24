using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Runtime.Serialization;
using System.Windows.Forms;

namespace FSM
{
    [Serializable]
    public class Event : Selectable
    {
        public delegate void RemoveDelegate(Event sender);
        [field: NonSerializedAttribute] public event RemoveDelegate RemoveEvent;
        
        static private Font mFont = new Font(FontFamily.GenericSansSerif, 10, FontStyle.Regular);
        static private Brush mTextBrush = new SolidBrush(Color.Black);

        static private Pen mPenArrow = new Pen(Color.LightSlateGray, 1);
        static private Pen mPen = new Pen(Color.LightSlateGray, 1);
        static private Pen mPenSelArrow = new Pen(Color.Red, 2);
        static private Pen mPenSel = new Pen(Color.Red, 2);
        private static bool mInitializePens = true;

        [NonSerializedAttribute] private State mFrom = null;
        [NonSerializedAttribute] private State mTo = null;

        private ActionsList mActions = new ActionsList();
        private Point mVia = Point.Empty;

        private string mNameFrom = "";
        private string mNameTo = "";
        private Guard mGuard = new Guard();

        [OnSerializing()]
        internal void OnSerializingMethod(StreamingContext context)
        {
            mNameFrom = mFrom.Name;
            mNameTo = (mTo ==null) ? "" : mTo.Name;
        }

        [OnDeserialized()]
        internal void OnDeserializedMethod(StreamingContext context)
        {
            mTo = null;
            mFrom = null;
            
            foreach (var s in StateCollection.GetStates())
            {
                if (s.Name.Equals(mNameFrom))
                {
                    mFrom = s;
                    mFrom.RemoveEvent += Remove;
                }
                if (s.Name.Equals(mNameTo))
                {
                    mTo = s;
                    mTo.RemoveEvent += Remove;
                }
            }
            EventCollection.Add(this);
        }


        private void InitVia()
        {
            if (mFrom == mTo)
            {
                mVia = mFrom.FindConnector(Point.Empty);
				mVia.X -= 15;
				mVia.Y -= 15;
            }
            else
            {
                mVia = Point.Empty;
            }
        }
        public Event(State from, State to)
        {
            mFrom = from;
            mTo = to;
            InitVia();
            if (mFrom != null) mFrom.RemoveEvent += Remove;
            if (mTo != null) mTo.RemoveEvent += Remove;
        }

        public Guard Guard
        {
            get { return mGuard; }
            set { mGuard = value; }
        }
        override public void Remove()
        {
            if (mFrom != null)
            {
                mFrom.RemoveEvent -= Remove;
                mFrom = null;
            }
            if (mTo != null)
            {
                mTo.RemoveEvent -= Remove;
                mTo = null;
            }
            if (RemoveEvent != null) RemoveEvent(this);
        }

        private void Remove(State node)
        {
            Remove();
        }

        override public bool IsSelected(Point pos)
        {
            return Util.CalcDistance(Center,pos)<7;
        }

        public override void Move(int dx, int dy)
        {
            if (mVia == Point.Empty)
            {
                mVia = Center;
            }
            mVia.X += dx;
            mVia.Y += dy;
        }
        override public Point Center
        {
            get
            {
                if (!mVia.IsEmpty) return mVia;
                if (mTo == null) return mFrom.Center; 
                Point from = mFrom.FindConnector(mTo.Center);
                Point to = mTo.FindConnector(mFrom.Center, from);
                return new Point((from.X + to.X) / 2, (from.Y + to.Y) / 2);
            }
            set
            {
                mVia = value;
            }
        }
       
        public void Draw(Graphics g)
        {
            if (mFrom == null || mTo == null) return;
            if (mInitializePens)
            {
                mPenArrow.EndCap = LineCap.Custom;
                mPenArrow.CustomEndCap = new AdjustableArrowCap(6, 6);
                mPenSelArrow.EndCap = LineCap.Custom;
                mPenSelArrow.CustomEndCap = new AdjustableArrowCap(6, 6);
                mInitializePens = false;
            }
            
            Point from;
            Point to;
            Point via;
            if (mVia.IsEmpty)
            {
                from = mFrom.FindConnector(mTo.Center);
                to = mTo.FindConnector(mFrom.Center, from);
                via = new Point((from.X + to.X)/2, (from.Y + to.Y)/2);
            }
            else
            {
                from = mFrom.FindConnector(mVia);
                to = mTo.FindConnector(mVia, from);
                via = mVia;                
            }
            Point[] curvePoints = {from, via, to};
            g.DrawCurve(Selected ? mPenSelArrow : mPenArrow, curvePoints, 1.0f);
            g.DrawRectangle(Selected ? mPenSel : mPen, via.X - 3, via.Y - 3, 6, 6);
            string def = EventDefinition();
            uint nameH = (uint)g.MeasureString(def, mFont).Height;
            uint w = (uint)g.MeasureString(def + "\n" + mActions, mFont).Width;
            g.DrawString(def + "\n" + mActions, mFont, mTextBrush, via.X-w/2, via.Y+3);
            g.DrawLine(mPen, via.X-w/2, via.Y + 2 + nameH, via.X + w/2, via.Y + 2 +  nameH);
        }

        public string EventDefinition()
        {
            string res = Name;
            if (!mGuard.Expression.Equals("")) res += " [" + mGuard.Expression + "]";
            return res;
        }

       public override bool Modify()
       {
            FormEvent fe = new FormEvent(this);
            return DialogResult.OK == fe.ShowDialog();
        }
        public State From { get { return mFrom; } 
            set
            {
                if (mFrom == value) return;
                if (mFrom != null) mFrom.RemoveEvent -= Remove;
                mFrom = value;
                if (mFrom != null) mFrom.RemoveEvent += Remove;
                InitVia();
            } }
        public State To
        {
            get { return mTo; }
            set
            {
                if (mTo == value) return;
                if (mTo != null) mTo.RemoveEvent -= Remove;
                mTo = value;
                if (mTo != null) mTo.RemoveEvent += Remove;
                InitVia();
            }
        }

        public ActionsList Actions { get { return mActions; } set { mActions = value; } }


        override public void ResetPosition()
        {
            mVia = Point.Empty;
        }
    }
}
