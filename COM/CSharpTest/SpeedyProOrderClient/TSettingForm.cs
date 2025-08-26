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
            TSEBrokerIDBox.Text = Program.FTSEBrokerID;
            BrokerIDTextBox.Text = Program.FBrokerID;
            CMIDTextBox.Text = Program.FCMID;
            mmAccounttextBox.Text = Program.FMMAccount;
            DatatextBox.Text = Program.UserData;
            
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
            Program.FBrokerID = BrokerIDTextBox.Text;
            Program.FCMID = CMIDTextBox.Text;
            Program.FTSEBrokerID = TSEBrokerIDBox.Text;
            Program.FMMAccount = mmAccounttextBox.Text;
            Program.FNewVer = "true";
            Program.UserData = DatatextBox.Text;
            if (NewOrderCheckBox.Checked)
                Program.FNewOrder ="true";
            else
                Program.FNewOrder = "false";
        }
    }
}