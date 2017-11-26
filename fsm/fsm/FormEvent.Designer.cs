namespace FSM
{
    partial class FormEvent
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
            this.mFieldActions = new System.Windows.Forms.TextBox();
            this.mButtonCancel = new System.Windows.Forms.Button();
            this.mButtonOk = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.mDeleteButton = new System.Windows.Forms.Button();
            this.mTo = new System.Windows.Forms.ComboBox();
            this.mFrom = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.mFieldSelect = new System.Windows.Forms.ComboBox();
            this.mGuardText = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Name";
            // 
            // mFieldActions
            // 
            this.mFieldActions.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mFieldActions.Location = new System.Drawing.Point(73, 113);
            this.mFieldActions.Multiline = true;
            this.mFieldActions.Name = "mFieldActions";
            this.mFieldActions.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.mFieldActions.Size = new System.Drawing.Size(476, 129);
            this.mFieldActions.TabIndex = 4;
            // 
            // mButtonCancel
            // 
            this.mButtonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.mButtonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.mButtonCancel.Location = new System.Drawing.Point(474, 271);
            this.mButtonCancel.Name = "mButtonCancel";
            this.mButtonCancel.Size = new System.Drawing.Size(75, 23);
            this.mButtonCancel.TabIndex = 6;
            this.mButtonCancel.Text = "Cancel";
            this.mButtonCancel.UseVisualStyleBackColor = true;
            this.mButtonCancel.Click += new System.EventHandler(this.mButtonCancel_Click);
            // 
            // mButtonOk
            // 
            this.mButtonOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.mButtonOk.Location = new System.Drawing.Point(393, 271);
            this.mButtonOk.Name = "mButtonOk";
            this.mButtonOk.Size = new System.Drawing.Size(75, 23);
            this.mButtonOk.TabIndex = 5;
            this.mButtonOk.Text = "Ok";
            this.mButtonOk.UseVisualStyleBackColor = true;
            this.mButtonOk.Click += new System.EventHandler(this.mButtonOk_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 116);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(42, 13);
            this.label2.TabIndex = 12;
            this.label2.Text = "Actions";
            // 
            // mDeleteButton
            // 
            this.mDeleteButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.mDeleteButton.Location = new System.Drawing.Point(312, 271);
            this.mDeleteButton.Name = "mDeleteButton";
            this.mDeleteButton.Size = new System.Drawing.Size(75, 23);
            this.mDeleteButton.TabIndex = 7;
            this.mDeleteButton.Text = "Delete";
            this.mDeleteButton.UseVisualStyleBackColor = true;
            this.mDeleteButton.Click += new System.EventHandler(this.mDeleteButton_Click);
            // 
            // mTo
            // 
            this.mTo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mTo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.mTo.FormattingEnabled = true;
            this.mTo.Location = new System.Drawing.Point(73, 86);
            this.mTo.Name = "mTo";
            this.mTo.Size = new System.Drawing.Size(476, 21);
            this.mTo.TabIndex = 3;
            // 
            // mFrom
            // 
            this.mFrom.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mFrom.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.mFrom.FormattingEnabled = true;
            this.mFrom.Location = new System.Drawing.Point(73, 59);
            this.mFrom.Name = "mFrom";
            this.mFrom.Size = new System.Drawing.Size(476, 21);
            this.mFrom.TabIndex = 2;
            this.mFrom.SelectedIndexChanged += new System.EventHandler(this.mFrom_SelectedIndexChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 86);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(20, 13);
            this.label4.TabIndex = 18;
            this.label4.Text = "To";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 62);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(30, 13);
            this.label5.TabIndex = 17;
            this.label5.Text = "From";
            // 
            // mFieldSelect
            // 
            this.mFieldSelect.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mFieldSelect.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.mFieldSelect.FormattingEnabled = true;
            this.mFieldSelect.Location = new System.Drawing.Point(73, 6);
            this.mFieldSelect.Name = "mFieldSelect";
            this.mFieldSelect.Size = new System.Drawing.Size(476, 21);
            this.mFieldSelect.TabIndex = 1;
            // 
            // mGuardText
            // 
            this.mGuardText.Location = new System.Drawing.Point(73, 33);
            this.mGuardText.Name = "mGuardText";
            this.mGuardText.Size = new System.Drawing.Size(476, 20);
            this.mGuardText.TabIndex = 20;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 36);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(36, 13);
            this.label3.TabIndex = 21;
            this.label3.Text = "Guard";
            // 
            // FormEvent
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.mButtonCancel;
            this.ClientSize = new System.Drawing.Size(561, 306);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.mGuardText);
            this.Controls.Add(this.mFieldSelect);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.mTo);
            this.Controls.Add(this.mFrom);
            this.Controls.Add(this.mDeleteButton);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.mButtonCancel);
            this.Controls.Add(this.mButtonOk);
            this.Controls.Add(this.mFieldActions);
            this.Controls.Add(this.label1);
            this.Name = "FormEvent";
            this.Text = "Event";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox mFieldActions;
        private System.Windows.Forms.Button mButtonCancel;
        private System.Windows.Forms.Button mButtonOk;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button mDeleteButton;
        private System.Windows.Forms.ComboBox mTo;
        private System.Windows.Forms.ComboBox mFrom;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox mFieldSelect;
        private System.Windows.Forms.TextBox mGuardText;
        private System.Windows.Forms.Label label3;
    }
}