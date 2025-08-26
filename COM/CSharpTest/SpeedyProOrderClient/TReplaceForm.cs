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
        public TReplaceForm(int BeforeQty)
        {
            InitializeComponent();

            FBeforeQty.Text = BeforeQty.ToString();
            FBeforeQty.Enabled = false;
            BeforeQty--;
            FAfterQty.Text = BeforeQty.ToString();
        }

        private void FAfterQty_KeyDown(object sender, KeyEventArgs e)
        {
            Program.TextBoxNumberOnly( e, false );
            if( e.KeyCode == Keys.Enter )
            {
                EventArgs ea = new EventArgs();
                FOK_Click(this, ea);
            }
        }

        public int GetReplaceQty()
        {
            try
            {
                return Convert.ToInt32(FBeforeQty.Text) - Convert.ToInt32(FAfterQty.Text);
            }
            catch( System.Exception  )
            {
                return 0;
            }
        }

        private void FOK_Click(object sender, EventArgs e)
        {
            if( GetReplaceQty() <= 0 )
            {
                    System.Windows.Forms.MessageBox.Show( "Support reduce qty only.");
                    DialogResult = DialogResult.None;
            }
            else
                    DialogResult = DialogResult.OK;
        }
    }
}