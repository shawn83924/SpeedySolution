using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Test1
{
    public partial class TReplaceForm : Form
    {
        bool FIsReplaceQty = true;
        public TReplaceForm( SpeedyAPI.MarketEnum Market, int BeforeQty,double Px, bool IsLimitROD, bool IsPartiallyFill )
        {
            InitializeComponent();

            if (Market != SpeedyAPI.MarketEnum.mOTC && Market != SpeedyAPI.MarketEnum.mTSE)
            {
                if (IsLimitROD == false)
                    this.ReplacetabControl.TabPages.Remove(this.ReplacePxtabPage);
                if (IsPartiallyFill == false)
                {
                    TIFcomboBox.Items.Add("FOK");
                    TIFcomboBox.Items.Add("IOC");
                    TIFcomboBox.Enabled = true;
                }
                else
                {
                    TIFcomboBox.Items.Add("IOC");
                    TIFcomboBox.Enabled = false;
                }
                TIFcomboBox.SelectedIndex = 0;
                Px -=1;
                PxtextBox.Text = Px.ToString();
            }
            else
            {
                ReplacetabControl.TabPages.Remove(this.ReplacePxtabPage);
                FIsReplaceQty = true;
            }
            FBeforeQty.Text = BeforeQty.ToString();
            FBeforeQty.Enabled = false;
            int AfterQty = BeforeQty - 1;
            if( AfterQty > 0 )
                FAfterQty.Text = AfterQty.ToString();
        }

        private void FAfterQty_KeyDown(object sender, KeyEventArgs e)
        {
            Program.TextBoxNumberOnly( e, false );
        }

        public int GetReplaceQty()
        {
            try
            {
                return Convert.ToInt32(FBeforeQty.Text) - Convert.ToInt32(FAfterQty.Text);
            }
            catch( System.Exception )
            {
                return 0;
            }
        }
        public double GetReplacePrice()
        {
            try
            {
                return Convert.ToDouble( PxtextBox.Text );
            }
            catch (System.Exception)
            {
                return 0;
            }
        }
        public SpeedyAPI.TimeInForceEnum GetTIF()
        {
            if (LimitradioButton.Checked == true)
            {
                return SpeedyAPI.TimeInForceEnum.tifROD;
            }
            else
            {
                if (TIFcomboBox.SelectedIndex == 0)
                    return SpeedyAPI.TimeInForceEnum.tifFOK;
                else
                    return SpeedyAPI.TimeInForceEnum.tifIOC;                   
            }
        }
        public SpeedyAPI.OrderTypeEnum GetOrderType()
        {
            if (LimitradioButton.Checked == true)
                return SpeedyAPI.OrderTypeEnum.otLimit;
            else
                return SpeedyAPI.OrderTypeEnum.otMarket;
        }
        private void FOK_Click(object sender, EventArgs e)
        {
            if (ReplacetabControl.SelectedTab == ReplaceQtytabPage )
            {
                FIsReplaceQty = true;
                if (GetReplaceQty() <= 0)
                {
                    System.Windows.Forms.MessageBox.Show("Support reduce qty only.");
                    DialogResult = DialogResult.None;
                }
                else
                    DialogResult = DialogResult.OK;
            }
            else
            {
                FIsReplaceQty = false;
                if (LimitradioButton.Checked == true)
                {
                    if(GetReplacePrice()==0)
                    {
                        System.Windows.Forms.MessageBox.Show("Please input the price.");
                        DialogResult = DialogResult.None;
                    }
                    else
                        DialogResult = DialogResult.OK;
                }
                else
                {
                    DialogResult = DialogResult.OK;
                }                
            }
        }        
        public bool IsReplaceQty( )
        {
            return FIsReplaceQty;
        }
    }
}