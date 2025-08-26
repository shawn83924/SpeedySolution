namespace SpeedyClient
{
    partial class SelAPIForm
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
            this.Okbutton = new System.Windows.Forms.Button();
            this.APIcomboBox = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // Okbutton
            // 
            this.Okbutton.Location = new System.Drawing.Point(92, 64);
            this.Okbutton.Name = "Okbutton";
            this.Okbutton.Size = new System.Drawing.Size(90, 32);
            this.Okbutton.TabIndex = 0;
            this.Okbutton.Text = "OK";
            this.Okbutton.UseVisualStyleBackColor = true;
            this.Okbutton.Click += new System.EventHandler(this.Okbutton_Click);
            // 
            // APIcomboBox
            // 
            this.APIcomboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.APIcomboBox.FormattingEnabled = true;
            this.APIcomboBox.Location = new System.Drawing.Point(27, 24);
            this.APIcomboBox.Name = "APIcomboBox";
            this.APIcomboBox.Size = new System.Drawing.Size(220, 23);
            this.APIcomboBox.TabIndex = 1;
            // 
            // SelAPIForm
            // 
            this.AcceptButton = this.Okbutton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(276, 108);
            this.Controls.Add(this.APIcomboBox);
            this.Controls.Add(this.Okbutton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "SelAPIForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Select API";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button Okbutton;
        public System.Windows.Forms.ComboBox APIcomboBox;
    }
}