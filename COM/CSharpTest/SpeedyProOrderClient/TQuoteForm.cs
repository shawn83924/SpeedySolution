using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Test1
{
    public partial class TQuoteForm : Form
    {
        public TQuoteForm()
        {
            InitializeComponent();

            FTimeInForce.Items.Clear();
            FTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifROD);
            FTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifTFXQ);
        }

        private void FOK_Click(object sender, EventArgs e)
        {
            if ((FSymbol.Text.Length > 0) &&
                (FBidPrice.Text.Length > 0) &&
                (FBidQty.Text.Length > 0) &&
                (FAskPrice.Text.Length > 0) &&
                (FAskQty.Text.Length > 0))
            {
                DialogResult = DialogResult.OK;
            }
        }

        private void FBidPrice_KeyDown(object sender, KeyEventArgs e)
        {
            Program.TextBoxNumberOnly( e, true);
        }

        private void FBidQty_KeyDown(object sender, KeyEventArgs e)
        {
            Program.TextBoxNumberOnly(e, false);
        }

        public void SetSymbol( string Sym )
        {
            this.Text = "New Quote (" + Sym + ")";
            FSymbol.Text = Sym;
        }

        public void SetBidPrice( double price )
        {
            FBidPrice.Text = price.ToString();
            price *= 1.05;
            FAskPrice.Text = price.ToString();
        }

        public string GetSymbol()
        {
            return FSymbol.Text;
        }

        public double GetBidPrice()
        {
            return Program.ConvertStringToDouble(FBidPrice.Text);
        }

        public int GetBidQty()
        {
            return Program.ConvertStringToInt32(FBidQty.Text);
        }

        public double GetAskPrice()
        {
            return Program.ConvertStringToDouble(FAskPrice.Text);
        }

        public int GetAskQty()
        {
            return Program.ConvertStringToInt32(FAskQty.Text);
        }

        public OrderMessage.TimeInForceEnum GetTimeInForce()
        {            
            return ((OrderMessage.TimeInForceEnum)(FTimeInForce.SelectedItem));
        }

        private void TQuoteForm_Load(object sender, EventArgs e)
        {
            FTimeInForce.SelectedIndex = 0;
        }
    }
}