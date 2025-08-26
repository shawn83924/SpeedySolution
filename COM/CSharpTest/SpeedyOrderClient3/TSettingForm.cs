using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Test1
{
    public partial class SettingForm : Form
    {
        public SettingForm()
        {
            InitializeComponent();
        }

        private void SettingForm_Load(object sender, EventArgs e)
        {
            TSEBrokerIDBox.Text   = Program.FTSEBrokerID;
            BrokerIDTextBox.Text  = Program.FBrokerID;
            CNBrokerIDTextBox.Text = Program.FCNBrokerID;
            CMIDTextBox.Text      = Program.FCMID;
            mmAccounttextBox.Text = Program.FMMAccount;
            PATSAccountBox.Text   = Program.FPATSAccount;
            DataTextBox.Text      = Program.UserData;
            if (Program.FReadOnly == "true")
            {
                Program.FNewOrder = "false";
                NewOrderCheckBox.Checked = false;
                NewOrderCheckBox.Enabled = false;
            }
            else
            {
                NewOrderCheckBox.Enabled = true;
                if (Program.FNewOrder == "false")
                    NewOrderCheckBox.Checked = false;
                else
                    NewOrderCheckBox.Checked = true;
            }
        }

        private void FLogon_Click(object sender, EventArgs e)
        {
            Program.FBrokerID    = BrokerIDTextBox.Text;
            Program.FCNBrokerID  = CNBrokerIDTextBox.Text;
            Program.FTSEBrokerID = TSEBrokerIDBox.Text;
            Program.FCMID        = CMIDTextBox.Text;
            Program.FMMAccount   = mmAccounttextBox.Text;
            Program.FPATSAccount = PATSAccountBox.Text;
            Program.UserData     = DataTextBox.Text;
            Program.FNewVer = "true";
            if (NewOrderCheckBox.Checked)
                Program.FNewOrder ="true";
            else
                Program.FNewOrder = "false";
        }
    }
}