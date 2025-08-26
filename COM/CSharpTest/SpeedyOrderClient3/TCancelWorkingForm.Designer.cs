namespace SpeedyClient
{
    partial class CancelWorkingForm
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
            this.MarketComboBox = new System.Windows.Forms.ComboBox();
            this.label9 = new System.Windows.Forms.Label();
            this.AccountTextBox = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.AETextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.DataTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.ProdIDTextBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.FOK = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // MarketComboBox
            // 
            this.MarketComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.MarketComboBox.FormattingEnabled = true;
            this.MarketComboBox.Location = new System.Drawing.Point(105, 23);
            this.MarketComboBox.Name = "MarketComboBox";
            this.MarketComboBox.Size = new System.Drawing.Size(130, 25);
            this.MarketComboBox.TabIndex = 74;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(25, 26);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(53, 17);
            this.label9.TabIndex = 73;
            this.label9.Text = "Market";
            // 
            // AccountTextBox
            // 
            this.AccountTextBox.ForeColor = System.Drawing.SystemColors.WindowText;
            this.AccountTextBox.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.AccountTextBox.Location = new System.Drawing.Point(105, 84);
            this.AccountTextBox.MaxLength = 20;
            this.AccountTextBox.Name = "AccountTextBox";
            this.AccountTextBox.Size = new System.Drawing.Size(130, 28);
            this.AccountTextBox.TabIndex = 79;
            this.AccountTextBox.Text = "all";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(25, 87);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(62, 17);
            this.label16.TabIndex = 78;
            this.label16.Text = "Account";
            // 
            // AETextBox
            // 
            this.AETextBox.ForeColor = System.Drawing.SystemColors.WindowText;
            this.AETextBox.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.AETextBox.Location = new System.Drawing.Point(105, 52);
            this.AETextBox.MaxLength = 20;
            this.AETextBox.Name = "AETextBox";
            this.AETextBox.Size = new System.Drawing.Size(130, 28);
            this.AETextBox.TabIndex = 81;
            this.AETextBox.Text = "all";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(25, 55);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(30, 17);
            this.label1.TabIndex = 80;
            this.label1.Text = "AE";
            // 
            // DataTextBox
            // 
            this.DataTextBox.ForeColor = System.Drawing.SystemColors.WindowText;
            this.DataTextBox.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.DataTextBox.Location = new System.Drawing.Point(105, 115);
            this.DataTextBox.MaxLength = 20;
            this.DataTextBox.Name = "DataTextBox";
            this.DataTextBox.Size = new System.Drawing.Size(130, 28);
            this.DataTextBox.TabIndex = 83;
            this.DataTextBox.Text = "all";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(25, 118);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(72, 17);
            this.label2.TabIndex = 82;
            this.label2.Text = "User Data";
            // 
            // ProdIDTextBox
            // 
            this.ProdIDTextBox.ForeColor = System.Drawing.SystemColors.WindowText;
            this.ProdIDTextBox.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.ProdIDTextBox.Location = new System.Drawing.Point(105, 146);
            this.ProdIDTextBox.MaxLength = 20;
            this.ProdIDTextBox.Name = "ProdIDTextBox";
            this.ProdIDTextBox.Size = new System.Drawing.Size(130, 28);
            this.ProdIDTextBox.TabIndex = 85;
            this.ProdIDTextBox.Text = "all";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(25, 149);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(55, 17);
            this.label3.TabIndex = 84;
            this.label3.Text = "ProdID";
            // 
            // FOK
            // 
            this.FOK.Location = new System.Drawing.Point(78, 187);
            this.FOK.Name = "FOK";
            this.FOK.Size = new System.Drawing.Size(90, 26);
            this.FOK.TabIndex = 86;
            this.FOK.Text = "OK";
            this.FOK.UseVisualStyleBackColor = true;
            this.FOK.Click += new System.EventHandler(this.FOK_Click);
            // 
            // CancelWorkingForm
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(257, 230);
            this.Controls.Add(this.FOK);
            this.Controls.Add(this.ProdIDTextBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.DataTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.AETextBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.AccountTextBox);
            this.Controls.Add(this.label16);
            this.Controls.Add(this.MarketComboBox);
            this.Controls.Add(this.label9);
            this.Font = new System.Drawing.Font("PMingLiU", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "CancelWorkingForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Cancel Working Orders";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.TextBox AETextBox;
        public System.Windows.Forms.TextBox AccountTextBox;
        public System.Windows.Forms.TextBox DataTextBox;
        public System.Windows.Forms.TextBox ProdIDTextBox;

        private System.Windows.Forms.ComboBox MarketComboBox;
        private System.Windows.Forms.Label label9;        
        private System.Windows.Forms.Label label16;        
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;        
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button FOK;
    }
}