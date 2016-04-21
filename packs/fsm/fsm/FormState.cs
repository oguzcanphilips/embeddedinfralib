using System;
using System.Linq;
using System.Windows.Forms;

namespace FSM
{
    public partial class FormState : Form
    {
        private State mState;
        public FormState(State state)
        {
            mState = state;
            InitializeComponent();

            mFieldName.Text = state.Name;
            mFieldEnter.Text = state.Enter.AsString;
            mFieldLeave.Text = state.Leave.AsString;
            mInitialStateCheckBox.Checked = state.Initial;
            
            mParentSelect.Items.Add(State.Null);
            foreach (State s in StateCollection.GetStates().Where(s => s != state))
            {
                mParentSelect.Items.Add(s);
            }
            if (mState.Parent == null)
            {
                mParentSelect.SelectedItem = State.Null;
            }
            else
            {
                mParentSelect.SelectedItem = state.Parent;
            }
            UpdateInternalEvents();
        }
        private void UpdateInternalEvents()
        {
            mInternalEventSelect.Items.Clear();
            foreach (Event ev in mState.GetInternalEvents())
            {
                mInternalEventSelect.Items.Add(ev);
                mInternalEventSelect.SelectedItem = ev;
            }
        }
        private void mButtonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void mButtonOk_Click(object sender, EventArgs e)
        {
            mState.Name = mFieldName.Text;
            mState.Enter.AsString = mFieldEnter.Text;
            mState.Leave.AsString = mFieldLeave.Text;
            mState.Initial = mInitialStateCheckBox.Checked;
            if (mParentSelect.SelectedItem == State.Null)
            {
                mState.Parent = null;
            }
            else
            {
                mState.Parent = (State)mParentSelect.SelectedItem;
            }
            DialogResult = DialogResult.OK;
            Close();
        }

        private void mDeleteButton_Click(object sender, EventArgs e)
        {
            mState.Remove();
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void AddEventButton_Click(object sender, EventArgs e)
        {
            Event ev = new Event(mState, null);
            if (ev.Modify())
            {
                EventCollection.Add(ev);
                mInternalEventSelect.Items.Add(ev);
                mInternalEventSelect.SelectedItem = ev;
            }
        }

        private void ChangeEventButton_Click(object sender, EventArgs e)
        {
            Event ev = (Event)mInternalEventSelect.SelectedItem;
            if (ev != null) ev.Modify();
            UpdateInternalEvents();
        }
    }
}
