namespace FSM
{
    partial class FormState
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
            this.label1 = new System.Windows.Forms.Label();
            this.mFieldName = new System.Windows.Forms.TextBox();
            this.mButtonOk = new System.Windows.Forms.Button();
            this.mButtonCancel = new System.Windows.Forms.Button();
            this.mDeleteButton = new System.Windows.Forms.Button();
            this.mInitialStateCheckBox = new System.Windows.Forms.CheckBox();
            this.mFieldLeave = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.mFieldEnter = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.label4 = new System.Windows.Forms.Label();
            this.mParentSelect = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.mInternalEventSelect = new System.Windows.Forms.ComboBox();
            this.AddEventButton = new System.Windows.Forms.Button();
            this.ChangeEventButton = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Name";
            // 
            // mFieldName
            // 
            this.mFieldName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mFieldName.Location = new System.Drawing.Point(53, 6);
            this.mFieldName.Name = "mFieldName";
            this.mFieldName.Size = new System.Drawing.Size(377, 20);
            this.mFieldName.TabIndex = 1;
            // 
            // mButtonOk
            // 
            this.mButtonOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.mButtonOk.Location = new System.Drawing.Point(278, 355);
            this.mButtonOk.Name = "mButtonOk";
            this.mButtonOk.Size = new System.Drawing.Size(75, 23);
            this.mButtonOk.TabIndex = 20;
            this.mButtonOk.Text = "Ok";
            this.mButtonOk.UseVisualStyleBackColor = true;
            this.mButtonOk.Click += new System.EventHandler(this.mButtonOk_Click);
            // 
            // mButtonCancel
            // 
            this.mButtonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.mButtonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.mButtonCancel.Location = new System.Drawing.Point(359, 355);
            this.mButtonCancel.Name = "mButtonCancel";
            this.mButtonCancel.Size = new System.Drawing.Size(75, 23);
            this.mButtonCancel.TabIndex = 21;
            this.mButtonCancel.Text = "Cancel";
            this.mButtonCancel.UseVisualStyleBackColor = true;
            this.mButtonCancel.Click += new System.EventHandler(this.mButtonCancel_Click);
            // 
            // mDeleteButton
            // 
            this.mDeleteButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.mDeleteButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.mDeleteButton.Location = new System.Drawing.Point(197, 355);
            this.mDeleteButton.Name = "mDeleteButton";
            this.mDeleteButton.Size = new System.Drawing.Size(75, 23);
            this.mDeleteButton.TabIndex = 22;
            this.mDeleteButton.Text = "Delete";
            this.mDeleteButton.UseVisualStyleBackColor = true;
            this.mDeleteButton.Click += new System.EventHandler(this.mDeleteButton_Click);
            // 
            // mInitialStateCheckBox
            // 
            this.mInitialStateCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.mInitialStateCheckBox.AutoSize = true;
            this.mInitialStateCheckBox.Location = new System.Drawing.Point(12, 332);
            this.mInitialStateCheckBox.Name = "mInitialStateCheckBox";
            this.mInitialStateCheckBox.Size = new System.Drawing.Size(78, 17);
            this.mInitialStateCheckBox.TabIndex = 10;
            this.mInitialStateCheckBox.Text = "Initial State";
            this.mInitialStateCheckBox.UseVisualStyleBackColor = true;
            // 
            // mFieldLeave
            // 
            this.mFieldLeave.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mFieldLeave.Location = new System.Drawing.Point(38, 0);
            this.mFieldLeave.Multiline = true;
            this.mFieldLeave.Name = "mFieldLeave";
            this.mFieldLeave.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.mFieldLeave.Size = new System.Drawing.Size(386, 103);
            this.mFieldLeave.TabIndex = 7;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(3, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(37, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Leave";
            // 
            // mFieldEnter
            // 
            this.mFieldEnter.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mFieldEnter.Location = new System.Drawing.Point(38, 3);
            this.mFieldEnter.Multiline = true;
            this.mFieldEnter.Name = "mFieldEnter";
            this.mFieldEnter.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.mFieldEnter.Size = new System.Drawing.Size(386, 101);
            this.mFieldEnter.TabIndex = 5;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(32, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Enter";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.Location = new System.Drawing.Point(6, 109);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.label2);
            this.splitContainer1.Panel1.Controls.Add(this.mFieldEnter);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.label3);
            this.splitContainer1.Panel2.Controls.Add(this.mFieldLeave);
            this.splitContainer1.Size = new System.Drawing.Size(427, 213);
            this.splitContainer1.SplitterDistance = 105;
            this.splitContainer1.TabIndex = 8;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(9, 35);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(38, 13);
            this.label4.TabIndex = 24;
            this.label4.Text = "Parent";
            // 
            // mParentSelect
            // 
            this.mParentSelect.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mParentSelect.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.mParentSelect.FormattingEnabled = true;
            this.mParentSelect.Location = new System.Drawing.Point(53, 32);
            this.mParentSelect.Name = "mParentSelect";
            this.mParentSelect.Size = new System.Drawing.Size(377, 21);
            this.mParentSelect.TabIndex = 25;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(10, 62);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(78, 13);
            this.label5.TabIndex = 26;
            this.label5.Text = "Internal Events";
            // 
            // mInternalEventSelect
            // 
            this.mInternalEventSelect.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mInternalEventSelect.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.mInternalEventSelect.FormattingEnabled = true;
            this.mInternalEventSelect.Location = new System.Drawing.Point(53, 82);
            this.mInternalEventSelect.Name = "mInternalEventSelect";
            this.mInternalEventSelect.Size = new System.Drawing.Size(377, 21);
            this.mInternalEventSelect.TabIndex = 27;
            // 
            // AddEventButton
            // 
            this.AddEventButton.Location = new System.Drawing.Point(91, 58);
            this.AddEventButton.Name = "AddEventButton";
            this.AddEventButton.Size = new System.Drawing.Size(60, 21);
            this.AddEventButton.TabIndex = 28;
            this.AddEventButton.Text = "Add";
            this.AddEventButton.UseVisualStyleBackColor = true;
            this.AddEventButton.Click += new System.EventHandler(this.AddEventButton_Click);
            // 
            // ChangeEventButton
            // 
            this.ChangeEventButton.Location = new System.Drawing.Point(157, 58);
            this.ChangeEventButton.Name = "ChangeEventButton";
            this.ChangeEventButton.Size = new System.Drawing.Size(60, 21);
            this.ChangeEventButton.TabIndex = 29;
            this.ChangeEventButton.Text = "Change";
            this.ChangeEventButton.UseVisualStyleBackColor = true;
            this.ChangeEventButton.Click += new System.EventHandler(this.ChangeEventButton_Click);
            // 
            // FormState
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.mButtonCancel;
            this.ClientSize = new System.Drawing.Size(444, 390);
            this.Controls.Add(this.ChangeEventButton);
            this.Controls.Add(this.AddEventButton);
            this.Controls.Add(this.mInternalEventSelect);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.mParentSelect);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.mInitialStateCheckBox);
            this.Controls.Add(this.mDeleteButton);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.mButtonCancel);
            this.Controls.Add(this.mButtonOk);
            this.Controls.Add(this.mFieldName);
            this.Controls.Add(this.label1);
            this.Name = "FormState";
            this.Text = "State";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox mFieldName;
        private System.Windows.Forms.Button mButtonOk;
        private System.Windows.Forms.Button mButtonCancel;
        private System.Windows.Forms.Button mDeleteButton;
        private System.Windows.Forms.CheckBox mInitialStateCheckBox;
        private System.Windows.Forms.TextBox mFieldLeave;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox mFieldEnter;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox mParentSelect;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox mInternalEventSelect;
        private System.Windows.Forms.Button AddEventButton;
        private System.Windows.Forms.Button ChangeEventButton;
    }
}