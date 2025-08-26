using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SpeedyClient
{
    public partial class CancelWorkingForm : Form
    {
        public CancelWorkingForm()
        {
            InitializeComponent();
            MarketComboBox.Items.Add(SpeedyAPI.CancelMarketEnum.cmAll);
            MarketComboBox.Items.Add(SpeedyAPI.CancelMarketEnum.cmTWFutures);
            MarketComboBox.Items.Add(SpeedyAPI.CancelMarketEnum.cmTWOptions );
            MarketComboBox.SelectedIndex = 0;
        }
        public SpeedyAPI.CancelMarketEnum GetMarket()
        {
            switch (MarketComboBox.SelectedIndex)
            {
                case 0: return SpeedyAPI.CancelMarketEnum.cmAll;
                case 1: return SpeedyAPI.CancelMarketEnum.cmTWFutures;
                case 2: return SpeedyAPI.CancelMarketEnum.cmTWOptions;
                default: return SpeedyAPI.CancelMarketEnum.cmAll;
            }
        }

        private void FOK_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }
    }
}
