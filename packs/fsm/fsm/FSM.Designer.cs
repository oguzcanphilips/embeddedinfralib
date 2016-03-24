namespace FSM
{
    partial class FSM
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mNewStateButton = new System.Windows.Forms.Button();
            this.mNewEventButton = new System.Windows.Forms.Button();
            this.mSaveButton = new System.Windows.Forms.Button();
            this.mLoadButton = new System.Windows.Forms.Button();
            this.mNameField = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.mGenerateButton = new System.Windows.Forms.Button();
            this.mCheckButton = new System.Windows.Forms.Button();
            this.mGenerateFolderDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.mSaveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.mOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.mScrollH = new System.Windows.Forms.HScrollBar();
            this.mScrollV = new System.Windows.Forms.VScrollBar();
            this.mCopyButton = new System.Windows.Forms.Button();
            this.mZoom = new System.Windows.Forms.HScrollBar();
            this.mCenterButton = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.mPanel = new DoubleBufferPanel();
            this.mFeedback = new System.Windows.Forms.TextBox();
            this.mCheckerButton = new System.Windows.Forms.Button();
            this.useAssertOnNestedEvents = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // mNewStateButton
            // 
            this.mNewStateButton.Location = new System.Drawing.Point(12, 92);
            this.mNewStateButton.Name = "mNewStateButton";
            this.mNewStateButton.Size = new System.Drawing.Size(90, 23);
            this.mNewStateButton.TabIndex = 1;
            this.mNewStateButton.Text = "+ State";
            this.mNewStateButton.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mNewStateButton.UseVisualStyleBackColor = true;
            this.mNewStateButton.Click += new System.EventHandler(this.mNewStateButton_Click);
            // 
            // mNewEventButton
            // 
            this.mNewEventButton.Location = new System.Drawing.Point(12, 121);
            this.mNewEventButton.Name = "mNewEventButton";
            this.mNewEventButton.Size = new System.Drawing.Size(90, 23);
            this.mNewEventButton.TabIndex = 2;
            this.mNewEventButton.Text = "+ Event";
            this.mNewEventButton.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mNewEventButton.UseVisualStyleBackColor = true;
            this.mNewEventButton.Click += new System.EventHandler(this.mNewEventButton_Click);
            // 
            // mSaveButton
            // 
            this.mSaveButton.Location = new System.Drawing.Point(12, 225);
            this.mSaveButton.Name = "mSaveButton";
            this.mSaveButton.Size = new System.Drawing.Size(90, 23);
            this.mSaveButton.TabIndex = 0;
            this.mSaveButton.Text = "Save";
            this.mSaveButton.UseVisualStyleBackColor = true;
            this.mSaveButton.Click += new System.EventHandler(this.mSaveButton_Click);
            // 
            // mLoadButton
            // 
            this.mLoadButton.Location = new System.Drawing.Point(12, 254);
            this.mLoadButton.Name = "mLoadButton";
            this.mLoadButton.Size = new System.Drawing.Size(90, 23);
            this.mLoadButton.TabIndex = 3;
            this.mLoadButton.Text = "Load";
            this.mLoadButton.UseVisualStyleBackColor = true;
            this.mLoadButton.Click += new System.EventHandler(this.mLoadButton_Click);
            // 
            // mNameField
            // 
            this.mNameField.Location = new System.Drawing.Point(12, 28);
            this.mNameField.Name = "mNameField";
            this.mNameField.Size = new System.Drawing.Size(90, 20);
            this.mNameField.TabIndex = 4;
            this.mNameField.TextChanged += new System.EventHandler(this.mNameField_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(38, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Name:";
            // 
            // mGenerateButton
            // 
            this.mGenerateButton.Location = new System.Drawing.Point(12, 365);
            this.mGenerateButton.Name = "mGenerateButton";
            this.mGenerateButton.Size = new System.Drawing.Size(90, 23);
            this.mGenerateButton.TabIndex = 6;
            this.mGenerateButton.Text = "Generate Code";
            this.mGenerateButton.UseVisualStyleBackColor = true;
            this.mGenerateButton.Click += new System.EventHandler(this.mGenerateButton_Click);
            // 
            // mCheckButton
            // 
            this.mCheckButton.Location = new System.Drawing.Point(12, 336);
            this.mCheckButton.Name = "mCheckButton";
            this.mCheckButton.Size = new System.Drawing.Size(90, 23);
            this.mCheckButton.TabIndex = 8;
            this.mCheckButton.Text = "Check";
            this.mCheckButton.UseVisualStyleBackColor = true;
            this.mCheckButton.Click += new System.EventHandler(this.mCheckButton_Click);
            // 
            // mSaveFileDialog
            // 
            this.mSaveFileDialog.DefaultExt = "fsm";
            this.mSaveFileDialog.Filter = "FiniteStatemachine|*.fsm|All files|*.*";
            // 
            // mOpenFileDialog
            // 
            this.mOpenFileDialog.DefaultExt = "fsm";
            this.mOpenFileDialog.Filter = "FiniteStatemachine|*.fsm|All files|*.*";
            // 
            // mScrollH
            // 
            this.mScrollH.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mScrollH.Location = new System.Drawing.Point(115, 440);
            this.mScrollH.Maximum = 1000;
            this.mScrollH.Name = "mScrollH";
            this.mScrollH.Size = new System.Drawing.Size(476, 18);
            this.mScrollH.TabIndex = 9;
            this.mScrollH.Value = 500;
            this.mScrollH.ValueChanged += new System.EventHandler(this.mScroll_ValueChanged);
            // 
            // mScrollV
            // 
            this.mScrollV.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mScrollV.Location = new System.Drawing.Point(591, 12);
            this.mScrollV.Maximum = 1000;
            this.mScrollV.Name = "mScrollV";
            this.mScrollV.Size = new System.Drawing.Size(18, 428);
            this.mScrollV.TabIndex = 10;
            this.mScrollV.Value = 500;
            this.mScrollV.ValueChanged += new System.EventHandler(this.mScroll_ValueChanged);
            // 
            // mCopyButton
            // 
            this.mCopyButton.Location = new System.Drawing.Point(12, 283);
            this.mCopyButton.Name = "mCopyButton";
            this.mCopyButton.Size = new System.Drawing.Size(90, 23);
            this.mCopyButton.TabIndex = 11;
            this.mCopyButton.Text = "To Clipboard";
            this.mCopyButton.UseVisualStyleBackColor = true;
            this.mCopyButton.Click += new System.EventHandler(this.mCopyButton_Click);
            // 
            // mZoom
            // 
            this.mZoom.LargeChange = 1;
            this.mZoom.Location = new System.Drawing.Point(12, 411);
            this.mZoom.Minimum = 10;
            this.mZoom.Name = "mZoom";
            this.mZoom.Size = new System.Drawing.Size(90, 18);
            this.mZoom.TabIndex = 15;
            this.mZoom.Value = 100;
            this.mZoom.Scroll += new System.Windows.Forms.ScrollEventHandler(this.mZoom_Scroll);
            this.mZoom.ValueChanged += new System.EventHandler(this.mZoom_Scroll);
            // 
            // mCenterButton
            // 
            this.mCenterButton.Location = new System.Drawing.Point(12, 179);
            this.mCenterButton.Name = "mCenterButton";
            this.mCenterButton.Size = new System.Drawing.Size(90, 23);
            this.mCenterButton.TabIndex = 16;
            this.mCenterButton.Text = "Center";
            this.mCenterButton.UseVisualStyleBackColor = true;
            this.mCenterButton.Click += new System.EventHandler(this.mCenterButton_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 394);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(37, 13);
            this.label3.TabIndex = 17;
            this.label3.Text = "Zoom:";
            // 
            // mPanel
            // 
            this.mPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mPanel.BackColor = System.Drawing.SystemColors.Window;
            this.mPanel.Location = new System.Drawing.Point(115, 12);
            this.mPanel.Name = "mPanel";
            this.mPanel.Size = new System.Drawing.Size(476, 428);
            this.mPanel.TabIndex = 0;
            this.mPanel.SizeChanged += new System.EventHandler(this.mPanel_SizeChanged);
            this.mPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.mPanel_Paint);
            this.mPanel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.mPanel_MouseDown);
            this.mPanel.MouseMove += new System.Windows.Forms.MouseEventHandler(this.mPanel_MouseMove);
            this.mPanel.MouseUp += new System.Windows.Forms.MouseEventHandler(this.mPanel_MouseUp);
            // 
            // mFeedback
            // 
            this.mFeedback.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mFeedback.Location = new System.Drawing.Point(12, 463);
            this.mFeedback.Multiline = true;
            this.mFeedback.Name = "mFeedback";
            this.mFeedback.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.mFeedback.Size = new System.Drawing.Size(593, 140);
            this.mFeedback.TabIndex = 18;
            // 
            // mCheckerButton
            // 
            this.mCheckerButton.Location = new System.Drawing.Point(12, 150);
            this.mCheckerButton.Name = "mCheckerButton";
            this.mCheckerButton.Size = new System.Drawing.Size(90, 23);
            this.mCheckerButton.TabIndex = 19;
            this.mCheckerButton.Text = "+ Check";
            this.mCheckerButton.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mCheckerButton.UseVisualStyleBackColor = true;
            this.mCheckerButton.Click += new System.EventHandler(this.mCheckerButton_Click);
            // 
            // useAssertOnNestedEvents
            // 
            this.useAssertOnNestedEvents.AutoSize = true;
            this.useAssertOnNestedEvents.Location = new System.Drawing.Point(15, 55);
            this.useAssertOnNestedEvents.Name = "useAssertOnNestedEvents";
            this.useAssertOnNestedEvents.Size = new System.Drawing.Size(76, 17);
            this.useAssertOnNestedEvents.TabIndex = 20;
            this.useAssertOnNestedEvents.Text = "Use assert";
            this.useAssertOnNestedEvents.UseVisualStyleBackColor = true;
            this.useAssertOnNestedEvents.CheckedChanged += new System.EventHandler(this.useAssertOnNestedEvents_CheckedChanged);
            // 
            // FSM
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(617, 615);
            this.Controls.Add(this.useAssertOnNestedEvents);
            this.Controls.Add(this.mCheckerButton);
            this.Controls.Add(this.mFeedback);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.mCenterButton);
            this.Controls.Add(this.mZoom);
            this.Controls.Add(this.mCopyButton);
            this.Controls.Add(this.mScrollV);
            this.Controls.Add(this.mScrollH);
            this.Controls.Add(this.mCheckButton);
            this.Controls.Add(this.mGenerateButton);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.mNameField);
            this.Controls.Add(this.mLoadButton);
            this.Controls.Add(this.mSaveButton);
            this.Controls.Add(this.mNewEventButton);
            this.Controls.Add(this.mNewStateButton);
            this.Controls.Add(this.mPanel);
            this.MinimumSize = new System.Drawing.Size(633, 653);
            this.Name = "FSM";
            this.Text = "FSM";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button mNewStateButton;
        private System.Windows.Forms.Button mNewEventButton;
        private DoubleBufferPanel mPanel;
        private System.Windows.Forms.Button mSaveButton;
        private System.Windows.Forms.Button mLoadButton;
        private System.Windows.Forms.TextBox mNameField;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button mGenerateButton;
        private System.Windows.Forms.Button mCheckButton;
        private System.Windows.Forms.FolderBrowserDialog mGenerateFolderDialog;
        private System.Windows.Forms.SaveFileDialog mSaveFileDialog;
        private System.Windows.Forms.OpenFileDialog mOpenFileDialog;
        private System.Windows.Forms.HScrollBar mScrollH;
        private System.Windows.Forms.VScrollBar mScrollV;
        private System.Windows.Forms.Button mCopyButton;
        private System.Windows.Forms.HScrollBar mZoom;
        private System.Windows.Forms.Button mCenterButton;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox mFeedback;
        private System.Windows.Forms.Button mCheckerButton;
        private System.Windows.Forms.CheckBox useAssertOnNestedEvents;
    }
}

