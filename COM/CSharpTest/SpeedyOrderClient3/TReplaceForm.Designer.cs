namespace Test1
{
    partial class TReplaceForm
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
            this.FOK = new System.Windows.Forms.Button();
            this.ReplacetabControl = new System.Windows.Forms.TabControl();
            this.ReplaceQtytabPage = new System.Windows.Forms.TabPage();
            this.label1 = new System.Windows.Forms.Label();
            this.FBeforeQty = new System.Windows.Forms.TextBox();
            this.FAfterQty = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.ReplacePxtabPage = new System.Windows.Forms.TabPage();
            this.TIFcomboBox = new System.Windows.Forms.ComboBox();
            this.PxtextBox = new System.Windows.Forms.TextBox();
            this.MarketradioButton = new System.Windows.Forms.RadioButton();
            this.LimitradioButton = new System.Windows.Forms.RadioButton();
            this.ReplacetabControl.SuspendLayout();
            this.ReplaceQtytabPage.SuspendLayout();
            this.ReplacePxtabPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // FOK
            // 
            this.FOK.Location = new System.Drawing.Point(75, 127);
            this.FOK.Name = "FOK";
            this.FOK.Size = new System.Drawing.Size(90, 26);
            this.FOK.TabIndex = 5;
            this.FOK.Text = "OK";
            this.FOK.UseVisualStyleBackColor = true;
            this.FOK.Click += new System.EventHandler(this.FOK_Click);
            // 
            // ReplacetabControl
            // 
            this.ReplacetabControl.Controls.Add(this.ReplaceQtytabPage);
            this.ReplacetabControl.Controls.Add(this.ReplacePxtabPage);
            this.ReplacetabControl.Location = new System.Drawing.Point(3, 3);
            this.ReplacetabControl.Name = "ReplacetabControl";
            this.ReplacetabControl.SelectedIndex = 0;
            this.ReplacetabControl.Size = new System.Drawing.Size(252, 122);
            this.ReplacetabControl.TabIndex = 6;
            // 
            // ReplaceQtytabPage
            // 
            this.ReplaceQtytabPage.Controls.Add(this.label1);
            this.ReplaceQtytabPage.Controls.Add(this.FBeforeQty);
            this.ReplaceQtytabPage.Controls.Add(this.FAfterQty);
            this.ReplaceQtytabPage.Controls.Add(this.label2);
            this.ReplaceQtytabPage.Location = new System.Drawing.Point(4, 30);
            this.ReplaceQtytabPage.Name = "ReplaceQtytabPage";
            this.ReplaceQtytabPage.Padding = new System.Windows.Forms.Padding(3);
            this.ReplaceQtytabPage.Size = new System.Drawing.Size(244, 88);
            this.ReplaceQtytabPage.TabIndex = 0;
            this.ReplaceQtytabPage.Text = "改量";
            this.ReplaceQtytabPage.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(87, 20);
            this.label1.TabIndex = 8;
            this.label1.Text = "BeforeQty";
            // 
            // FBeforeQty
            // 
            this.FBeforeQty.Location = new System.Drawing.Point(96, 12);
            this.FBeforeQty.MaxLength = 4;
            this.FBeforeQty.Name = "FBeforeQty";
            this.FBeforeQty.Size = new System.Drawing.Size(77, 31);
            this.FBeforeQty.TabIndex = 7;
            // 
            // FAfterQty
            // 
            this.FAfterQty.Location = new System.Drawing.Point(96, 52);
            this.FAfterQty.MaxLength = 4;
            this.FAfterQty.Name = "FAfterQty";
            this.FAfterQty.Size = new System.Drawing.Size(77, 31);
            this.FAfterQty.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 55);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(76, 20);
            this.label2.TabIndex = 5;
            this.label2.Text = "AfterQty";
            // 
            // ReplacePxtabPage
            // 
            this.ReplacePxtabPage.Controls.Add(this.TIFcomboBox);
            this.ReplacePxtabPage.Controls.Add(this.PxtextBox);
            this.ReplacePxtabPage.Controls.Add(this.MarketradioButton);
            this.ReplacePxtabPage.Controls.Add(this.LimitradioButton);
            this.ReplacePxtabPage.Location = new System.Drawing.Point(4, 30);
            this.ReplacePxtabPage.Name = "ReplacePxtabPage";
            this.ReplacePxtabPage.Padding = new System.Windows.Forms.Padding(3);
            this.ReplacePxtabPage.Size = new System.Drawing.Size(244, 88);
            this.ReplacePxtabPage.TabIndex = 1;
            this.ReplacePxtabPage.Text = "改價";
            this.ReplacePxtabPage.UseVisualStyleBackColor = true;
            // 
            // TIFcomboBox
            // 
            this.TIFcomboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.TIFcomboBox.FormattingEnabled = true;
            this.TIFcomboBox.Location = new System.Drawing.Point(142, 53);
            this.TIFcomboBox.Name = "TIFcomboBox";
            this.TIFcomboBox.Size = new System.Drawing.Size(86, 28);
            this.TIFcomboBox.TabIndex = 3;
            // 
            // PxtextBox
            // 
            this.PxtextBox.Location = new System.Drawing.Point(142, 15);
            this.PxtextBox.Name = "PxtextBox";
            this.PxtextBox.Size = new System.Drawing.Size(86, 31);
            this.PxtextBox.TabIndex = 2;
            // 
            // MarketradioButton
            // 
            this.MarketradioButton.AutoSize = true;
            this.MarketradioButton.Location = new System.Drawing.Point(24, 54);
            this.MarketradioButton.Name = "MarketradioButton";
            this.MarketradioButton.Size = new System.Drawing.Size(83, 24);
            this.MarketradioButton.TabIndex = 1;
            this.MarketradioButton.Text = "Market";
            this.MarketradioButton.UseVisualStyleBackColor = true;
            // 
            // LimitradioButton
            // 
            this.LimitradioButton.AutoSize = true;
            this.LimitradioButton.Checked = true;
            this.LimitradioButton.Location = new System.Drawing.Point(24, 18);
            this.LimitradioButton.Name = "LimitradioButton";
            this.LimitradioButton.Size = new System.Drawing.Size(118, 24);
            this.LimitradioButton.TabIndex = 0;
            this.LimitradioButton.TabStop = true;
            this.LimitradioButton.Text = "Limit Price:";
            this.LimitradioButton.UseVisualStyleBackColor = true;
            // 
            // TReplaceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(257, 156);
            this.Controls.Add(this.ReplacetabControl);
            this.Controls.Add(this.FOK);
            this.Font = new System.Drawing.Font("PMingLiU", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "TReplaceForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Change Qty";
            this.ReplacetabControl.ResumeLayout(false);
            this.ReplaceQtytabPage.ResumeLayout(false);
            this.ReplaceQtytabPage.PerformLayout();
            this.ReplacePxtabPage.ResumeLayout(false);
            this.ReplacePxtabPage.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button FOK;
        private System.Windows.Forms.TabControl ReplacetabControl;
        private System.Windows.Forms.TabPage ReplaceQtytabPage;
        private System.Windows.Forms.TabPage ReplacePxtabPage;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox FBeforeQty;
        private System.Windows.Forms.TextBox FAfterQty;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox TIFcomboBox;
        private System.Windows.Forms.TextBox PxtextBox;
        private System.Windows.Forms.RadioButton MarketradioButton;
        private System.Windows.Forms.RadioButton LimitradioButton;
    }
}