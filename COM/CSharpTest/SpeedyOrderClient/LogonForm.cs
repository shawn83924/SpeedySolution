using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

/*
 * TextBox
 * =======
 * IP:          OrderServer的IP Address
 * Port:        OrderServer的Port
 * ID:          登入的使用者名稱
 * PASS:        登入的驗證密碼
 * ACNO:        投資人的交易所帳號
 * 
 * 
 * 
 * Button
 * ======
 * Connect:     建立與OrderServer的連線
 * Logon:       送出登入訊息到OrderServer
 * 
 * 
 * 
 * Label
 * =====
 * Status:      目前狀態的文字敘述
 * 
 * 
 * 
 * Timer
 * ==========
 * CloseTimer:  1秒後關閉程式的計時器
 * 
 * */

namespace Test1
{
    public partial class LogonForm : Form
    {
        OrderConnection.TaifexConnection FOrderConnection = null;        
        //---------------------------------------------------------------------------
        public LogonForm(OrderConnection.TaifexConnection conn)
        {
            InitializeComponent();            
            FOrderConnection = conn;
        }        
        //---------------------------------------------------------------------------
        private void FLogon_Click(object sender, EventArgs e)
        {
            if (ProxyCheckBox.Checked == false)
            {
                if (FId.Text.Length <= 0)
                {
                    MessageBox.Show("Id.Text.Length <= 0");
                    this.DialogResult = DialogResult.None;
                    return;
                }
                if (FAccount.Text.Length <= 0)
                {
                    MessageBox.Show("Acno.Text.Length <= 0");
                    this.DialogResult = DialogResult.None;
                    return;
                }
                Program.FLogonAccount = FAccount.Text;
                Program.FLogonID = FId.Text;
                FOrderConnection.Logon(FId.Text, FPass.Text, FAccount.Text, OrderConnection.ConnectionTypeEnum.ctBoth);
            }
            else
            {
                if (FAccount.Text.Length <= 0)
                {
                    MessageBox.Show("Acno.Text.Length <= 0");
                    this.DialogResult = DialogResult.None;
                    return;
                }
                Program.FLogonAccount = FAccount.Text;
                Program.FLogonID = FId.Text;
                FOrderConnection.LogonProxy( FId.Text, FPass.Text, FAccount.Text, "SpeedyProxy", "601008");
            }
        }
        //---------------------------------------------------------------------------
    }
}