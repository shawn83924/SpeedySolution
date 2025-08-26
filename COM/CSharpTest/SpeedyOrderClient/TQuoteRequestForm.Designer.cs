namespace SpeedyClient
{
    partial class TQuoteRequestForm
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
            this.FSymbol = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // FOK
            // 
            this.FOK.Location = new System.Drawing.Point(235, 11);
            this.FOK.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.FOK.Name = "FOK";
            this.FOK.Size = new System.Drawing.Size(88, 29);
            this.FOK.TabIndex = 33;
            this.FOK.Text = "OK";
            this.FOK.UseVisualStyleBackColor = true;
            // 
            // FSymbol
            // 
            this.FSymbol.Location = new System.Drawing.Point(71, 12);
            this.FSymbol.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.FSymbol.Name = "FSymbol";
            this.FSymbol.Size = new System.Drawing.Size(156, 27);
            this.FSymbol.TabIndex = 32;
            this.FSymbol.Text = "TXO06600L6";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(13, 15);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(56, 16);
            this.label3.TabIndex = 31;
            this.label3.Text = "Symbol";
            // 
            // TQuoteRequestForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(343, 56);
            this.Controls.Add(this.FOK);
            this.Controls.Add(this.FSymbol);
            this.Controls.Add(this.label3);
            this.Font = new System.Drawing.Font("PMingLiU", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "TQuoteRequestForm";
            this.Text = "Quote Request";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button FOK;
        private System.Windows.Forms.TextBox FSymbol;
        private System.Windows.Forms.Label label3;
    }
}