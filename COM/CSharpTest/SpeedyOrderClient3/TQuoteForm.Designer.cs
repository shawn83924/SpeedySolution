namespace Test1
{
    partial class TQuoteForm
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
            this.FSymbol = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.FBidQty = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.FBidPrice = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.FAskQty = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.FAskPrice = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.FOK = new System.Windows.Forms.Button();
            this.FTimeInForce = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // FSymbol
            // 
            this.FSymbol.Location = new System.Drawing.Point(149, -5);
            this.FSymbol.Name = "FSymbol";
            this.FSymbol.Size = new System.Drawing.Size(120, 27);
            this.FSymbol.TabIndex = 8;
            this.FSymbol.Text = "TXO06600L6";
            this.FSymbol.Visible = false;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(144, -20);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(56, 16);
            this.label3.TabIndex = 7;
            this.label3.Text = "Symbol";
            // 
            // FBidQty
            // 
            this.FBidQty.Location = new System.Drawing.Point(262, 13);
            this.FBidQty.MaxLength = 4;
            this.FBidQty.Name = "FBidQty";
            this.FBidQty.Size = new System.Drawing.Size(45, 27);
            this.FBidQty.TabIndex = 23;
            this.FBidQty.Text = "10";
            this.FBidQty.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FBidQty_KeyDown);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(201, 16);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(53, 16);
            this.label12.TabIndex = 22;
            this.label12.Text = "BidQty";
            // 
            // FBidPrice
            // 
            this.FBidPrice.Location = new System.Drawing.Point(88, 13);
            this.FBidPrice.MaxLength = 10;
            this.FBidPrice.Name = "FBidPrice";
            this.FBidPrice.Size = new System.Drawing.Size(100, 27);
            this.FBidPrice.TabIndex = 21;
            this.FBidPrice.Text = "430";
            this.FBidPrice.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FBidPrice_KeyDown);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(16, 16);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(61, 16);
            this.label8.TabIndex = 20;
            this.label8.Text = "BidPrice";
            // 
            // FAskQty
            // 
            this.FAskQty.Location = new System.Drawing.Point(262, 46);
            this.FAskQty.MaxLength = 4;
            this.FAskQty.Name = "FAskQty";
            this.FAskQty.Size = new System.Drawing.Size(45, 27);
            this.FAskQty.TabIndex = 27;
            this.FAskQty.Text = "10";
            this.FAskQty.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FBidQty_KeyDown);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(201, 49);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 16);
            this.label1.TabIndex = 26;
            this.label1.Text = "AskQty";
            // 
            // FAskPrice
            // 
            this.FAskPrice.Location = new System.Drawing.Point(88, 46);
            this.FAskPrice.MaxLength = 10;
            this.FAskPrice.Name = "FAskPrice";
            this.FAskPrice.Size = new System.Drawing.Size(100, 27);
            this.FAskPrice.TabIndex = 25;
            this.FAskPrice.Text = "440";
            this.FAskPrice.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FBidPrice_KeyDown);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(13, 49);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(64, 16);
            this.label2.TabIndex = 24;
            this.label2.Text = "AskPrice";
            // 
            // FOK
            // 
            this.FOK.Location = new System.Drawing.Point(219, 89);
            this.FOK.Name = "FOK";
            this.FOK.Size = new System.Drawing.Size(100, 27);
            this.FOK.TabIndex = 30;
            this.FOK.Text = "OK";
            this.FOK.UseVisualStyleBackColor = true;
            this.FOK.Click += new System.EventHandler(this.FOK_Click);
            // 
            // FTimeInForce
            // 
            this.FTimeInForce.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.FTimeInForce.FormattingEnabled = true;
            this.FTimeInForce.Location = new System.Drawing.Point(107, 92);
            this.FTimeInForce.Name = "FTimeInForce";
            this.FTimeInForce.Size = new System.Drawing.Size(96, 24);
            this.FTimeInForce.TabIndex = 29;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(13, 96);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(88, 16);
            this.label11.TabIndex = 28;
            this.label11.Text = "TimeInForce";
            // 
            // TQuoteForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(345, 135);
            this.Controls.Add(this.FTimeInForce);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.FOK);
            this.Controls.Add(this.FAskQty);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.FAskPrice);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.FBidQty);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.FBidPrice);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.FSymbol);
            this.Controls.Add(this.label3);
            this.Font = new System.Drawing.Font("PMingLiU", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "TQuoteForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "New Quote";
            this.Load += new System.EventHandler(this.TQuoteForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox FSymbol;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox FBidQty;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox FBidPrice;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox FAskQty;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox FAskPrice;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button FOK;
        private System.Windows.Forms.ComboBox FTimeInForce;
        private System.Windows.Forms.Label label11;
    }
}