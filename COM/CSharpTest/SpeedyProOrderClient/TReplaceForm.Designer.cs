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
            this.label2 = new System.Windows.Forms.Label();
            this.FAfterQty = new System.Windows.Forms.TextBox();
            this.FBeforeQty = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.FOK = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 49);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(63, 16);
            this.label2.TabIndex = 1;
            this.label2.Text = "AfterQty";
            // 
            // FAfterQty
            // 
            this.FAfterQty.Location = new System.Drawing.Point(91, 46);
            this.FAfterQty.MaxLength = 4;
            this.FAfterQty.Name = "FAfterQty";
            this.FAfterQty.Size = new System.Drawing.Size(77, 27);
            this.FAfterQty.TabIndex = 2;
            this.FAfterQty.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FAfterQty_KeyDown);
            // 
            // FBeforeQty
            // 
            this.FBeforeQty.Location = new System.Drawing.Point(91, 6);
            this.FBeforeQty.MaxLength = 4;
            this.FBeforeQty.Name = "FBeforeQty";
            this.FBeforeQty.Size = new System.Drawing.Size(77, 27);
            this.FBeforeQty.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(73, 16);
            this.label1.TabIndex = 4;
            this.label1.Text = "BeforeQty";
            // 
            // FOK
            // 
            this.FOK.Location = new System.Drawing.Point(182, 46);
            this.FOK.Name = "FOK";
            this.FOK.Size = new System.Drawing.Size(90, 26);
            this.FOK.TabIndex = 5;
            this.FOK.Text = "OK";
            this.FOK.UseVisualStyleBackColor = true;
            this.FOK.Click += new System.EventHandler(this.FOK_Click);
            // 
            // TReplaceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(287, 88);
            this.Controls.Add(this.FOK);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.FBeforeQty);
            this.Controls.Add(this.FAfterQty);
            this.Controls.Add(this.label2);
            this.Font = new System.Drawing.Font("PMingLiU", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "TReplaceForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Change Qty";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox FAfterQty;
        private System.Windows.Forms.TextBox FBeforeQty;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button FOK;
    }
}