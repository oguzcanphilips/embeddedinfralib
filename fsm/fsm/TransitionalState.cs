using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FSM
{
    [Serializable]
    class TransitionalState : State
    {
        private static readonly int DrawSize = 10;
        private static readonly Pen mBorderPen = new Pen(Color.Black, 1);
        static private readonly Pen mBorderPenSel = new Pen(Color.Red, 3);
        private static readonly Brush mBackGround = new SolidBrush(Color.FromArgb(250, 250, 210));

        public TransitionalState()
        {
            string name = "TransitionalState";
            int id=0;
            while (StateCollection.GetState(name+id) != null)
            {
                id++;
            }
            Name = name+id;
        }
        override public bool IsSelected(Point pos)
        {
            double dx = Center.X - pos.X;
            double dy = Center.Y - pos.Y;
            return (dx * dx + dy * dy) <= (DrawSize * DrawSize);
        }
        public override void Move(int dx, int dy)
        {
            Point center = Center;
            center.X += dx;
            center.Y += dy;
            Center = center;
        }
        override public void Draw(Graphics g)
        {
            GraphicsPath gp = new GraphicsPath();
            gp.StartFigure();
            gp.AddPolygon(new Point[]
                {
                    new Point(Center.X-DrawSize, Center.Y),
                    new Point(Center.X, Center.Y-DrawSize),
                    new Point(Center.X+DrawSize, Center.Y),
                    new Point(Center.X, Center.Y+DrawSize),
                });
            gp.CloseFigure();
            g.FillPath(mBackGround, gp);
            g.DrawPath(Selected ? mBorderPenSel : mBorderPen, gp);
        }
        override public Point FindConnector(Point from, Point exclude)
        {
            Point[] points = new Point[]
            {
                new Point(Center.X-DrawSize, Center.Y),
                new Point(Center.X, Center.Y-DrawSize),
                new Point(Center.X+DrawSize, Center.Y),
                new Point(Center.X, Center.Y+DrawSize)
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

        public override bool Modify()
        {
            if (MessageBox.Show("Delete?", "Delete", MessageBoxButtons.OKCancel) == DialogResult.Cancel) return false;
            Remove();
            return true;
        }
    }
}
