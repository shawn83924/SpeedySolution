namespace Test1
{
    partial class LogonForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.FLogon = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.FId = new System.Windows.Forms.TextBox();
            this.FPass = new System.Windows.Forms.TextBox();
            this.FAccount = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // FLogon
            // 
            this.FLogon.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.FLogon.Location = new System.Drawing.Point(68, 127);
            this.FLogon.Name = "FLogon";
            this.FLogon.Size = new System.Drawing.Size(126, 34);
            this.FLogon.TabIndex = 12;
            this.FLogon.Text = "Logon";
            this.FLogon.UseVisualStyleBackColor = true;
            this.FLogon.Click += new System.EventHandler(this.FLogon_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(24, 16);
            this.label1.TabIndex = 23;
            this.label1.Text = "ID";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(14, 48);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(67, 16);
            this.label2.TabIndex = 24;
            this.label2.Text = "Password";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(15, 85);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(61, 16);
            this.label3.TabIndex = 25;
            this.label3.Text = "Account";
            // 
            // FId
            // 
            this.FId.Location = new System.Drawing.Point(99, 12);
            this.FId.Name = "FId";
            this.FId.Size = new System.Drawing.Size(140, 27);
            this.FId.TabIndex = 3;
            this.FId.Text = "frank";
            // 
            // FPass
            // 
            this.FPass.Location = new System.Drawing.Point(99, 48);
            this.FPass.Name = "FPass";
            this.FPass.PasswordChar = '*';
            this.FPass.Size = new System.Drawing.Size(140, 27);
            this.FPass.TabIndex = 4;
            this.FPass.Text = "frank";
            // 
            // FAccount
            // 
            this.FAccount.Location = new System.Drawing.Point(99, 85);
            this.FAccount.Name = "FAccount";
            this.FAccount.Size = new System.Drawing.Size(140, 27);
            this.FAccount.TabIndex = 5;
            this.FAccount.Text = "0000001";
            // 
            // LogonForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(267, 171);
            this.Controls.Add(this.FAccount);
            this.Controls.Add(this.FPass);
            this.Controls.Add(this.FId);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.FLogon);
            this.Font = new System.Drawing.Font("PMingLiU", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "LogonForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "LogonForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button FLogon;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox FId;
        public System.Windows.Forms.TextBox FPass;
        public System.Windows.Forms.TextBox FAccount;
    }
}