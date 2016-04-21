using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace FSM
{
    public partial class FSM : Form
    {
        private Bitmap mPanelBitmap = new Bitmap(4000,6000);
        private Selectable mSelectedItem;
        private Point mPrevMousePos;
        private Config mConfig = new Config();
        private bool mCreateEvent = false;
        private State mNewEventFrom = null;
        private Point mNewEventTo = Point.Empty;

        public FSM(Config config = null)
        {
            mConfig = config ?? new Config();
            InitializeComponent();
            mNameField.Text = mConfig.Name;
            useAssertOnNestedEvents.Checked = mConfig.UseAssert;
            MouseWheel += FSM_MouseWheel;
            Text += @"   Philips Innovation Services - Software Products & Systems";
        }

        private Point ApplyScrollOffset(Point p)
        {
            p.X += mScrollH.Value * mPanelBitmap.Size.Width / mScrollH.Maximum;
            p.Y += mScrollV.Value * mPanelBitmap.Size.Height / mScrollV.Maximum;
            return p;
        }
        private Point ApplyScrollOffsetZoom(Point p)
        {
            p = ApplyScrollOffset(p);
            p.X *= mZoom.Maximum;
            p.Y *= mZoom.Maximum;
            p.X /= mZoom.Value;
            p.Y /= mZoom.Value;
            return p;
        }

        private void mPanel_Paint(object sender, PaintEventArgs e)
        {
            // Draw the mPanelBitmap into the panel
            using (Graphics bufferGrph = Graphics.FromImage(mPanelBitmap))
            {
                bufferGrph.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;
                bufferGrph.Clear(Color.White);
                foreach (var s in StateCollection.GetStates())
                {
                    s.Draw(bufferGrph);
                }
                foreach (var ev in EventCollection.GetEvents())
                {
                    ev.Draw(bufferGrph);
                }
                if (mCreateEvent && mNewEventFrom != null) 
                    bufferGrph.DrawLine(new Pen(Color.Blue, 1), mNewEventFrom.FindConnector(mNewEventTo), mNewEventTo);
            }
            e.Graphics.Clear(Color.Gray);
            Point origin = ApplyScrollOffset(Point.Empty);
            origin.X = -origin.X;
            origin.Y = -origin.Y;
            Rectangle dest = new Rectangle(
                origin.X, origin.Y,
                mZoom.Value * mPanelBitmap.Width / mZoom.Maximum, 
                mZoom.Value * mPanelBitmap.Height / mZoom.Maximum);
            e.Graphics.DrawImage(mPanelBitmap,dest);
        }
        
        private void AddNode(State node)
        {
            StateCollection.Add(node);
            mPanel.Invalidate();
            UpdateFeedback();
        }

        private void AddEvent(Event ev)
        {
            EventCollection.Add(ev);
            mPanel.Invalidate();
            UpdateFeedback();
        }
        private void RemoveEvent(Event ev)
        {
            ev.Remove();
            UpdateFeedback();
        }
        
        private void mNewStateButton_Click(object sender, EventArgs e)
        {
            State newState = new State();
            var fs = new FormState(newState);
            if (DialogResult.OK != fs.ShowDialog()) return;
            if (newState.Name.Length == 0) return;
            if (StateCollection.GetStates().Any(state => state.Name.Equals(newState.Name))) return;
            newState.Center = ApplyScrollOffsetZoom(newState.Center);
            AddNode(newState);
        }

        private void mNewEventButton_Click(object sender, EventArgs e)
        {
            mCreateEvent = true;
        }

        private Selectable FindSelectable(Point pos)
        {
            Selectable s = FindEvent(pos);
            if (s != null) return s;
            s = FindNode(pos);
            return s;
        }

        private Event FindEvent(Point pos)
        {
            Event sel = null;
            foreach (var s in EventCollection.GetEvents().Where(s => s.IsSelected(pos)))
            {
                sel = s;
            }
            return sel;
        }
        private State FindNode(Point pos)
        {
            State sel = null;
            foreach (var s in StateCollection.GetStates().Where(s => s.IsSelected(pos)))
            {
                sel = s;
            }
            return sel;
        }

        private void mPanel_MouseDown(object sender, MouseEventArgs e)
        {
            Point loc = ApplyScrollOffsetZoom(e.Location);
            Selectable sel = FindSelectable(loc);
            if (sel != null)
            {
                switch (e.Button)
                {
                    case MouseButtons.Left:
                        mSelectedItem = sel;
                        mSelectedItem.Selected = true;
                        if (mCreateEvent && sel is State)
                        {
                            mNewEventFrom = (State) sel;
                            mNewEventTo = loc;
                        }
                        break;
                    case MouseButtons.Right:
                        sel.Selected = true;
                        mPanel.Invalidate();
                        sel.Modify();
                        UpdateFeedback();
                        sel.Selected = false;
                        break;
                }
                mPrevMousePos = loc;
                mPanel.Invalidate();
            }
        }

        private void ReleaseSelectedItem()
        {
            if (mSelectedItem == null) return;
            mSelectedItem.Selected = false;
            mSelectedItem = null;
            mPanel.Invalidate();
        }

        private void mPanel_MouseUp(object sender, MouseEventArgs e)
        {
            Point loc = ApplyScrollOffsetZoom(e.Location);
            if (mCreateEvent)
            {
                State sel = FindNode(loc);
                if (sel != null)
                {
                    Event ev = new Event(mNewEventFrom, sel);
                    if (ev.Modify()) AddEvent(ev);
                    else RemoveEvent(ev);
                    sel.Selected = false;                    
                }
                
                mCreateEvent = false;
                mNewEventFrom = null;
            }
            ReleaseSelectedItem();
        }
        private bool IsPositionInPanel(Point pos)
        {
            if (pos.X < 0 || 
                pos.Y < 0 ||
                pos.X >= mPanel.Width || 
                pos.Y >= mPanel.Height) return false;
            return true;
        }
        private void mPanel_MouseMove(object sender, MouseEventArgs e)
        {
            Point loc = ApplyScrollOffsetZoom(e.Location);
            if (mCreateEvent)
            {
                State s = FindNode(mNewEventTo);
                if(s != null )s.Selected = false;
                mNewEventTo = loc;
                s = FindNode(mNewEventTo);
                if (s != null) s.Selected = true;
                mPanel.Invalidate();
                return;
            }
            if (mSelectedItem == null || !mSelectedItem.Selected) return;
            if (IsPositionInPanel(e.Location))
            {
                mSelectedItem.Move(loc.X - mPrevMousePos.X, loc.Y - mPrevMousePos.Y);
                mPrevMousePos = loc;
            }
            else
            {
                mSelectedItem.ResetPosition();
                ReleaseSelectedItem();
            }
            mPanel.Invalidate();
        }

        private void mSaveButton_Click(object sender, EventArgs e)
        {
            mSaveFileDialog.FileName = Persistent.LastFilename;
            if (DialogResult.OK != mSaveFileDialog.ShowDialog())
            {
                return;
            }
            Persistent.Save(mSaveFileDialog.FileName, mConfig);            
        }

        private void SetViewToCenter()
        {
            int xMin = mPanelBitmap.Width;
            int xMax = 0;
            int yMin = mPanelBitmap.Height;
            int yMax = 0;
            foreach (var node in StateCollection.GetStates())
            {
                int x = node.Center.X;
                int y = node.Center.Y;
                xMax = x > xMax ? x : xMax;
                xMin = x < xMin ? x : xMin;
                yMax = y > yMax ? y : yMax;
                yMin = y < yMin ? y : yMin;                
            }
            int h = (xMin + xMax)/2;
            h *= mZoom.Value;
            h /= mZoom.Maximum;
            h -= mPanel.Width/2;
            h *= mScrollH.Maximum;
            h /= mPanelBitmap.Width;
            if (h < mScrollH.Minimum) h = mScrollH.Minimum;
            if (h > mScrollH.Maximum) h = mScrollH.Maximum;
            mScrollH.Value = h;

            int v = (yMin + yMax)/2;
            v *= mZoom.Value;
            v /= mZoom.Maximum;
            v -= mPanel.Height/2;
            v *= mScrollV.Maximum;
            v /= mPanelBitmap.Height;
            if (v < mScrollV.Minimum) v = mScrollV.Minimum;
            if (v > mScrollV.Maximum) v = mScrollV.Maximum;
            mScrollV.Value = v;
        }

        private void mLoadButton_Click(object sender, EventArgs e)
        {
            if (DialogResult.OK != mOpenFileDialog.ShowDialog())
            {
                return;
            }
            try
            {
                mConfig = Persistent.Load(mOpenFileDialog.FileName);
                mNameField.Text = mConfig.Name;
                useAssertOnNestedEvents.Checked = mConfig.UseAssert;
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Load error", MessageBoxButtons.OK, MessageBoxIcon.Error);                
            }
            SetViewToCenter();
            mPanel.Invalidate();
            UpdateFeedback();
        }

        private void UpdateFeedback()
        {
            mFeedback.Text = ModelChecker.CheckModel(mConfig);
        }

        private Rectangle FindVisibleArea()
        {
            Rectangle rect = new Rectangle(Point.Empty, mPanelBitmap.Size);
            BitmapData data = null;
            int xvMax = 1;
            int yvMax = 1;
            int xvMin = mPanelBitmap.Width - 1;
            int yvMin = mPanelBitmap.Height - 1;
            try
            {
                data = mPanelBitmap.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

                byte[] rgbValues = new byte[data.Stride * data.Height];

                // Copy the RGB values into the array.
                System.Runtime.InteropServices.Marshal.Copy(data.Scan0, rgbValues, 0, rgbValues.Length);

                for (int i = 0; i < rgbValues.Length; ++i)
                {
                    if (rgbValues[i] != 0xff)
                    {
                        int x = (i % data.Stride) / 3;
                        int y = i / data.Stride;
                        if (x >= xvMax) xvMax = x + 5;
                        if (y >= yvMax) yvMax = y + 5;
                        if (x <= xvMin) xvMin = x - 5;
                        if (y <= yvMin) yvMin = y - 5;
                    }
                }
            }
            finally
            {
                if (data != null) mPanelBitmap.UnlockBits(data);
            }
            if (xvMin < 0) xvMin = 0;
            if (yvMin < 0) yvMin = 0;
            if (xvMax >= mPanelBitmap.Width) xvMax = mPanelBitmap.Width - 1;
            if (yvMax >= mPanelBitmap.Height) yvMax = mPanelBitmap.Height - 1;
            if (xvMax <= xvMin) xvMin = 0;
            if (yvMax <= yvMin) yvMin = 0;
            Rectangle viewArea = new Rectangle(xvMin, yvMin, xvMax - xvMin + 1, yvMax - yvMin + 1);
            return viewArea;
        }

        private void mCopyButton_Click(object sender, EventArgs e)
        {
            Rectangle cropArea = FindVisibleArea();
            Clipboard.SetImage(mPanelBitmap.Clone(cropArea, mPanelBitmap.PixelFormat));
        }
        
        private void mNameField_TextChanged(object sender, EventArgs e)
        {
            mConfig.Name = mNameField.Text;
        }

        private void mGenerateButton_Click(object sender, EventArgs e)
        {
            string err = ModelChecker.CheckModel(mConfig);
            if (err.Length > 0)
            {
                MessageBox.Show(err, "Model Check", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else if (DialogResult.OK == mGenerateFolderDialog.ShowDialog())
            {
                OutputFile.SetRoot(mGenerateFolderDialog.SelectedPath);
                new GeneratorCPP(mConfig);
                new GeneratorC(mConfig);
            }
        }
        private void mCheckButton_Click(object sender, EventArgs e)
        {            
            string err = ModelChecker.CheckModel(mConfig);
            mFeedback.Text = err; 
            if (err.Length > 0)
            {
                MessageBox.Show(err, "Model Check", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                MessageBox.Show("No errors found", "Model Check", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
        private void mScroll_ValueChanged(object sender, EventArgs e)
        {
            Invalidate(true);
        }

        private void mCenterButton_Click(object sender, EventArgs e)
        {
            SetViewToCenter();
        }

        private void mZoom_Scroll(object sender, EventArgs e)
        {
            SetViewToCenter();
        }

        private void FSM_MouseWheel(object sender, MouseEventArgs e)
        {
            if (mSelectedItem == null || !mSelectedItem.Selected)
            {
                int zoom = mZoom.Value;
                if (e.Delta > 0) zoom += 10;
                if (e.Delta < 0) zoom -= 10;
                if (zoom < mZoom.Minimum) zoom = mZoom.Minimum;
                if (zoom > mZoom.Maximum) zoom = mZoom.Maximum;
                mZoom.Value = zoom;
            }
            else
            {
                if (e.Delta > 0) mSelectedItem.Up();
                if (e.Delta < 0) mSelectedItem.Down();
                mPanel.Invalidate();
            }
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            base.OnClosing(e);

            bool ask = false;
            if (Persistent.LastFilename.Equals(""))
            {
                ask = true;
            }
            else
            {
                string originalName = Persistent.LastFilename;
                byte[] oldContent = File.ReadAllBytes(Persistent.LastFilename);
                Persistent.Save(Persistent.LastFilename + ".tmp", mConfig);
                byte[] newContent = File.ReadAllBytes(Persistent.LastFilename);
                File.Delete(Persistent.LastFilename);
                Persistent.LastFilename = originalName;
                if (oldContent.Length != newContent.Length)
                {
                    ask = true;
                }
                else
                {
                    for (int i = 0; i < oldContent.Length; ++i)
                    {
                        if (oldContent[i] != newContent[i])
                        {
                            ask = true;
                            break;
                        }
                    }
                }
            }

            if(ask && MessageBox.Show("Do you want to save changes?", "",
                MessageBoxButtons.YesNo) ==  DialogResult.Yes)
            {
                e.Cancel = true;
                mSaveButton_Click(null,null);

            }
        }

        private void mPanel_SizeChanged(object sender, EventArgs e)
        {
            mCenterButton_Click(sender,e);
        }

        private void mCheckerButton_Click(object sender, EventArgs e)
        {
            TransitionalState newState = new TransitionalState();
            newState.Center = ApplyScrollOffsetZoom(newState.Center);
            AddNode(newState);
        }

        private void useAssertOnNestedEvents_CheckedChanged(object sender, EventArgs e)
        {
            mConfig.UseAssert = useAssertOnNestedEvents.Checked;
        }

        private void mPanel_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (mSelectedItem != null && mSelectedItem.Selected)
            {
                mSelectedItem.Up();
                mPanel.Invalidate();
            }
        }
    }
}
