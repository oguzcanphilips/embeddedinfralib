using System;
using System.Drawing;

namespace FSM
{
    class Util
    {
        static public double CalcDistance(Point a, Point b)
        {
            int dx = (a.X - b.X);
            int dy = (a.Y - b.Y);
            return Math.Sqrt(dx * dx + dy * dy);
        }
    }
}
