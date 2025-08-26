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
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public SettingForm()
        {
            InitializeComponent();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void SettingForm_Load(object sender, EventArgs e)
        {
            TSEBrokerIDBox.Text   = Program.FTSEBrokerID;
            BrokerIDTextBox.Text  = Program.FBrokerID;
            CMIDTextBox.Text      = Program.FCMID;
            mmAccounttextBox.Text = Program.FMMAccount;
            DataTextBox.Text      = Program.UserData;
            SpeedyIP.Text         = Program.FSpeedyIP;
            SpeedyPort.Text       = Convert.ToString(Program.FSpeedyPort);
            StarWaveIP.Text       = Program.FStarWaveIP;
            StarWavePort.Text     = Convert.ToString(Program.FStarWavePort);
            ChartServerIP.Text    = Program.FChartServerIP;
            ChartServerPort.Text  = Convert.ToString(Program.FChartServerPort);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FLogon_Click(object sender, EventArgs e)
        {
            Program.FBrokerID    = BrokerIDTextBox.Text;
            Program.FCMID        = CMIDTextBox.Text;
            Program.FTSEBrokerID = TSEBrokerIDBox.Text;
            Program.FMMAccount   = mmAccounttextBox.Text;
            Program.UserData     = DataTextBox.Text;
            Program.FNewVer = "true";
            Program.FNewOrder ="true";
            Program.FSpeedyIP =  SpeedyIP.Text;
            Program.FSpeedyPort = Convert.ToInt32(SpeedyPort.Text);
            Program.FStarWaveIP = StarWaveIP.Text ;
            Program.FStarWavePort = Convert.ToInt32(StarWavePort.Text);
            Program.FChartServerIP = ChartServerIP.Text;
            Program.FChartServerPort = Convert.ToInt32(ChartServerPort.Text);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
    }
}