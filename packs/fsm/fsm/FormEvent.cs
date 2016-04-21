using System;
using System.Windows.Forms;

namespace FSM
{
    public partial class FormEvent : Form
    {
        private Event mEvent;        
        public FormEvent(Event ev)
        {
            mEvent = ev;
            InitializeComponent();
            
            mFieldActions.Text = mEvent.Actions.AsString;
            DialogResult = DialogResult.Cancel;
            mButtonOk.Enabled = false;
            foreach (var state in StateCollection.GetStates())
            {
                mButtonOk.Enabled = true;
                mFrom.Items.Add(state);
                mTo.Items.Add(state);
            }
            if (ev.From != null) mFrom.SelectedItem = ev.From;
            if (ev.To != null) mTo.SelectedItem = ev.To;
            else
            {
                mFrom.Hide();
                mTo.Hide();
                mButtonOk.Enabled = true;
            }

            mFieldSelect.DropDownStyle = ComboBoxStyle.Simple;

            if (mEvent.From.GetType() == typeof(TransitionalState))
            {
                mFieldSelect.Enabled = false;
            }
            else if (mEvent.Name!= null && !mEvent.Name.Equals(""))
            {
                mFieldSelect.Text = mEvent.Name;
            }
            mGuardText.Text = mEvent.Guard.Expression;
        }

        private void mButtonOk_Click(object sender, EventArgs e)
        {
            mEvent.Name = mFieldSelect.Text;
            mEvent.Actions.AsString = mFieldActions.Text;
            if(mFrom.SelectedItem != null) mEvent.From = (State)mFrom.SelectedItem;
            if (mTo.SelectedItem != null) mEvent.To = (State)mTo.SelectedItem;
            mEvent.Guard.Expression = mGuardText.Text;

            if (mEvent.From.GetType() == typeof (TransitionalState))
            {
                mEvent.Name = "";
            }
            DialogResult = DialogResult.OK;
            Close();
        }

        private void mButtonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void mDeleteButton_Click(object sender, EventArgs e)
        {
            mEvent.Remove();
            DialogResult = DialogResult.Cancel; 
            Close();
        }

        private void mFrom_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (mFrom.SelectedItem == null) 
                return;
            if (mFrom.SelectedItem.GetType() == typeof(TransitionalState))
            {
                mFieldSelect.Text = "";
                mFieldSelect.Enabled = false;
            }
            else
            {
                mFieldSelect.Enabled = true;
            }
        }
    }
}
