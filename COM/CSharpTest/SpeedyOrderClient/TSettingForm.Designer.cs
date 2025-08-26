namespace Test1
{
    partial class SettingForm
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.BrokerIDTextBox = new System.Windows.Forms.TextBox();
            this.CMIDTextBox = new System.Windows.Forms.TextBox();
            this.FLogon = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.TSEBrokerIDBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.NewOrderCheckBox = new System.Windows.Forms.CheckBox();
            this.mmAccounttextBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.DataTextBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.PATSAccountBox = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.CNBrokerIDTextBox = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(102, 50);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(85, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "Broker ID";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(41, 126);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(147, 20);
            this.label2.TabIndex = 1;
            this.label2.Text = "Clear Member ID ";
            // 
            // BrokerIDTextBox
            // 
            this.BrokerIDTextBox.Location = new System.Drawing.Point(189, 47);
            this.BrokerIDTextBox.MaxLength = 7;
            this.BrokerIDTextBox.Name = "BrokerIDTextBox";
            this.BrokerIDTextBox.Size = new System.Drawing.Size(112, 31);
            this.BrokerIDTextBox.TabIndex = 2;
            // 
            // CMIDTextBox
            // 
            this.CMIDTextBox.Location = new System.Drawing.Point(189, 123);
            this.CMIDTextBox.MaxLength = 4;
            this.CMIDTextBox.Name = "CMIDTextBox";
            this.CMIDTextBox.Size = new System.Drawing.Size(79, 31);
            this.CMIDTextBox.TabIndex = 3;
            // 
            // FLogon
            // 
            this.FLogon.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.FLogon.Location = new System.Drawing.Point(87, 309);
            this.FLogon.Name = "FLogon";
            this.FLogon.Size = new System.Drawing.Size(110, 32);
            this.FLogon.TabIndex = 13;
            this.FLogon.Text = "Ok";
            this.FLogon.UseVisualStyleBackColor = true;
            this.FLogon.Click += new System.EventHandler(this.FLogon_Click);
            // 
            // button1
            // 
            this.button1.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button1.Location = new System.Drawing.Point(202, 309);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(111, 32);
            this.button1.TabIndex = 14;
            this.button1.Text = "Cancel";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // TSEBrokerIDBox
            // 
            this.TSEBrokerIDBox.Location = new System.Drawing.Point(189, 8);
            this.TSEBrokerIDBox.MaxLength = 7;
            this.TSEBrokerIDBox.Name = "TSEBrokerIDBox";
            this.TSEBrokerIDBox.Size = new System.Drawing.Size(112, 31);
            this.TSEBrokerIDBox.TabIndex = 15;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(102, 11);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(85, 20);
            this.label3.TabIndex = 16;
            this.label3.Text = "Broker ID";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(308, 50);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(87, 20);
            this.label4.TabIndex = 17;
            this.label4.Text = "(TAIFEX)";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(306, 14);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(96, 20);
            this.label5.TabIndex = 18;
            this.label5.Text = "(TSE,OTC)";
            // 
            // NewOrderCheckBox
            // 
            this.NewOrderCheckBox.AutoSize = true;
            this.NewOrderCheckBox.Location = new System.Drawing.Point(105, 278);
            this.NewOrderCheckBox.Margin = new System.Windows.Forms.Padding(4);
            this.NewOrderCheckBox.Name = "NewOrderCheckBox";
            this.NewOrderCheckBox.Size = new System.Drawing.Size(183, 24);
            this.NewOrderCheckBox.TabIndex = 20;
            this.NewOrderCheckBox.Text = "Can place new order";
            this.NewOrderCheckBox.UseVisualStyleBackColor = true;
            // 
            // mmAccounttextBox
            // 
            this.mmAccounttextBox.Location = new System.Drawing.Point(189, 159);
            this.mmAccounttextBox.MaxLength = 7;
            this.mmAccounttextBox.Name = "mmAccounttextBox";
            this.mmAccounttextBox.Size = new System.Drawing.Size(112, 31);
            this.mmAccounttextBox.TabIndex = 21;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 162);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(180, 20);
            this.label6.TabIndex = 22;
            this.label6.Text = "Market maker Account";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(89, 237);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(99, 20);
            this.label7.TabIndex = 24;
            this.label7.Text = "User Define";
            // 
            // DataTextBox
            // 
            this.DataTextBox.Location = new System.Drawing.Point(189, 234);
            this.DataTextBox.MaxLength = 0;
            this.DataTextBox.Name = "DataTextBox";
            this.DataTextBox.Size = new System.Drawing.Size(195, 31);
            this.DataTextBox.TabIndex = 23;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(65, 199);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(121, 20);
            this.label8.TabIndex = 26;
            this.label8.Text = "PATS Account";
            // 
            // PATSAccountBox
            // 
            this.PATSAccountBox.ImeMode = System.Windows.Forms.ImeMode.Alpha;
            this.PATSAccountBox.Location = new System.Drawing.Point(189, 196);
            this.PATSAccountBox.MaxLength = 20;
            this.PATSAccountBox.Name = "PATSAccountBox";
            this.PATSAccountBox.Size = new System.Drawing.Size(195, 31);
            this.PATSAccountBox.TabIndex = 25;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(308, 87);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(65, 20);
            this.label9.TabIndex = 29;
            this.label9.Text = "(China)";
            // 
            // CNBrokerIDTextBox
            // 
            this.CNBrokerIDTextBox.Location = new System.Drawing.Point(189, 84);
            this.CNBrokerIDTextBox.MaxLength = 7;
            this.CNBrokerIDTextBox.Name = "CNBrokerIDTextBox";
            this.CNBrokerIDTextBox.Size = new System.Drawing.Size(112, 31);
            this.CNBrokerIDTextBox.TabIndex = 28;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(102, 87);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(85, 20);
            this.label10.TabIndex = 27;
            this.label10.Text = "Broker ID";
            // 
            // SettingForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(401, 375);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.CNBrokerIDTextBox);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.PATSAccountBox);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.DataTextBox);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.mmAccounttextBox);
            this.Controls.Add(this.NewOrderCheckBox);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.TSEBrokerIDBox);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.FLogon);
            this.Controls.Add(this.CMIDTextBox);
            this.Controls.Add(this.BrokerIDTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Font = new System.Drawing.Font("PMingLiU", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximizeBox = false;
            this.Name = "SettingForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Setting";
            this.Load += new System.EventHandler(this.SettingForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox BrokerIDTextBox;
        private System.Windows.Forms.TextBox CMIDTextBox;
        private System.Windows.Forms.Button FLogon;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox TSEBrokerIDBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckBox NewOrderCheckBox;
        private System.Windows.Forms.TextBox mmAccounttextBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox DataTextBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox PATSAccountBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox CNBrokerIDTextBox;
        private System.Windows.Forms.Label label10;
    }
}