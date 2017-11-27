using System.Windows.Forms;

public class DoubleBufferPanel : Panel
{
    public DoubleBufferPanel()
    {
        SetStyle(
            ControlStyles.AllPaintingInWmPaint |
            ControlStyles.UserPaint |
            ControlStyles.OptimizedDoubleBuffer, true);
        UpdateStyles();
    }
}
